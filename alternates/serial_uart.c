#include "quantum.h"
#include "serial.h"
#include "print.h"

#include "ch.h"
#include "chbsem.h"
#include "hal.h"

//#define xxprintf(...) dprintf(__VA_ARGS__)
#define xxprintf(...)

#ifndef USE_GPIOV1
// The default PAL alternate modes are used to signal that the pins are used for USART
#    ifndef SERIAL_UART_TX_PAL_MODE
#        define SERIAL_UART_TX_PAL_MODE 7
#    endif
#endif

#ifndef SERIAL_UART_DRIVER
#    define SERIAL_UART_DRIVER UARTD1
#endif

#ifndef SELECT_SOFT_SERIAL_SPEED
#    define SELECT_SOFT_SERIAL_SPEED 1
#endif

#ifdef SERIAL_UART_SPEED
// Allow advanced users to directly set SERIAL_UART_SPEED
#elif SELECT_SOFT_SERIAL_SPEED == 0
#    define SERIAL_UART_SPEED 230400
#elif SELECT_SOFT_SERIAL_SPEED == 1
#    define SERIAL_UART_SPEED 115200
#elif SELECT_SOFT_SERIAL_SPEED == 2
#    define SERIAL_UART_SPEED 57600
#elif SELECT_SOFT_SERIAL_SPEED == 3
#    define SERIAL_UART_SPEED 38400
#elif SELECT_SOFT_SERIAL_SPEED == 4
#    define SERIAL_UART_SPEED 19200
#elif SELECT_SOFT_SERIAL_SPEED == 5
#    define SERIAL_UART_SPEED 14400
#else
#    error invalid SELECT_SOFT_SERIAL_SPEED value
#endif

#ifndef SERIAL_UART_CR1
#    define SERIAL_UART_CR1 (USART_CR1_PCE | USART_CR1_PS | USART_CR1_M0)  // parity enable, odd parity, 9 bit length
#endif

#ifndef SERIAL_UART_CR2
#    define SERIAL_UART_CR2 (USART_CR2_STOP_1)  // 2 stop bits
#endif

#ifndef SERIAL_UART_CR3
#    define SERIAL_UART_CR3 0
#endif

#define TIMEOUT 10
#define HANDSHAKE_MAGIC 7

BSEMAPHORE_DECL(send_semaphore, 1);
BSEMAPHORE_DECL(recv_semaphore, 1);

static void callbackTransmitBufferEmpty(UARTDriver* uartp);
static void callbackReceiveError(UARTDriver* uartp, uartflags_t e);
static void callbackReceiveEnd(UARTDriver* uartp);

static const UARTConfig uartCfg = {
    NULL,                                // buffer-wise end-of-transmission
    callbackTransmitBufferEmpty,         // physical end-of-transmission
    callbackReceiveEnd,                  // end-of-receive
    NULL,                                // character received
    callbackReceiveError,                // error during receive
    NULL,                                // receiver timeout callback
    0,                                   // timeout
    (SERIAL_UART_SPEED),                 // speed - mandatory
    (SERIAL_UART_CR1),                   // CR1
    (SERIAL_UART_CR2),                   // CR2
    (SERIAL_UART_CR3) | USART_CR3_HDSEL  // CR3
};

void handle_soft_serial_slave(void);

static THD_WORKING_AREA(waSlaveThread, 2048);
static THD_FUNCTION(SlaveThread, arg) {
    (void)arg;
    chRegSetThreadName("slave_transport");

    while (true) {
        handle_soft_serial_slave();
    }
}

void uart_init(void) {
#if defined(USE_GPIOV1)
    palSetPadMode(PAL_PORT(SOFT_SERIAL_PIN), PAL_PAD(SOFT_SERIAL_PIN), PAL_MODE_STM32_ALTERNATE_OPENDRAIN);
#else
    palSetPadMode(PAL_PORT(SOFT_SERIAL_PIN), PAL_PAD(SOFT_SERIAL_PIN), PAL_MODE_ALTERNATE(SERIAL_UART_TX_PAL_MODE) | PAL_STM32_OTYPE_OPENDRAIN);
#endif

    uartStart(&SERIAL_UART_DRIVER, &uartCfg);
}

void uart_slave_init(void) {
    uart_init();

    // Start transport thread
    chThdCreateStatic(waSlaveThread, sizeof(waSlaveThread), HIGHPRIO, SlaveThread, NULL);
}

static void callbackTransmitBufferEmpty(UARTDriver* uart) {
    (void)uart;
    chSysLockFromISR();
    chBSemSignalI(&send_semaphore);
    chSysUnlockFromISR();
}

static void callbackReceiveEnd(UARTDriver* uart) {
    (void)uart;
    chSysLockFromISR();
    chBSemSignalI(&recv_semaphore);
    chSysUnlockFromISR();
}

static msg_t uart_recv(uint8_t* buf, size_t len, sysinterval_t timeout) {
    chBSemReset(&recv_semaphore, 1);
    xxprintf("starting recv(%d bytes)...\n", (int)len);
    uartStartReceive(&SERIAL_UART_DRIVER, len, buf);
    msg_t res = chBSemWaitTimeout(&recv_semaphore, timeout);
    xxprintf("recv complete\n");
    return res;
}

static msg_t uart_get(sysinterval_t timeout) {
    uint8_t c;
    msg_t   res = uart_recv(&c, 1, timeout);
    xxprintf("recv=0x%02X\n", (int)c);
    return res != MSG_OK ? res : c;
}

static msg_t uart_send(const uint8_t* buf, size_t len, sysinterval_t timeout) {
    chBSemReset(&send_semaphore, 1);
    xxprintf("starting send(%d bytes)...\n", (int)len);
    uartStartSend(&SERIAL_UART_DRIVER, len, buf);
    msg_t res = chBSemWaitTimeout(&send_semaphore, timeout);
    xxprintf("send complete\n");
    return res;
}

static msg_t uart_put(uint8_t c, sysinterval_t timeout) {
    xxprintf("send=0x%02X\n", (int)c);
    return uart_send(&c, 1, timeout);
}

static void callbackReceiveError(UARTDriver* uart, uartflags_t flags) {
    (void)uart;
    (void)flags;
}

static SSTD_t* Transaction_table      = NULL;
static uint8_t Transaction_table_size = 0;

void soft_serial_initiator_init(SSTD_t* sstd_table, int sstd_table_size) {
    Transaction_table      = sstd_table;
    Transaction_table_size = (uint8_t)sstd_table_size;

    uart_init();
}

void soft_serial_target_init(SSTD_t* sstd_table, int sstd_table_size) {
    Transaction_table      = sstd_table;
    Transaction_table_size = (uint8_t)sstd_table_size;

    uart_slave_init();
}

void handle_soft_serial_slave(void) {
    msg_t res = uart_get(TIME_INFINITE);  // first chunk is always transaction id
    if (res < 0) return;

    uint8_t sstd_index = (uint8_t)res;
    SSTD_t* trans      = &Transaction_table[sstd_index];

    // Always write back the sstd_index as part of a basic handshake
    sstd_index ^= HANDSHAKE_MAGIC;
    res = uart_put(sstd_index, TIME_INFINITE);
    if (res < 0) return;

    if (trans->initiator2target_buffer_size) {
        res = uart_recv(trans->initiator2target_buffer, trans->initiator2target_buffer_size, TIME_INFINITE);
        if (res < 0) return;
    }

    if (trans->target2initiator_buffer_size) {
        res = uart_send(trans->target2initiator_buffer, trans->target2initiator_buffer_size, TIME_INFINITE);
        if (res < 0) return;
    }

    if (trans->status) {
        *trans->status = TRANSACTION_ACCEPTED;
    }
}

/////////
//  start transaction by initiator
//
// int  soft_serial_transaction(int sstd_index)
//
// Returns:
//    TRANSACTION_END
//    TRANSACTION_NO_RESPONSE
//    TRANSACTION_DATA_ERROR
#ifndef SERIAL_USE_MULTI_TRANSACTION
int soft_serial_transaction(void) {
    uint8_t sstd_index = 0;
#else
int soft_serial_transaction(int index) {
    uint8_t sstd_index = index;
#endif

    if (sstd_index > Transaction_table_size) return TRANSACTION_TYPE_ERROR;
    SSTD_t* trans = &Transaction_table[sstd_index];
    msg_t   res   = 0;

    // First chunk is always transaction id
    res = uart_put(sstd_index, TIME_MS2I(TIMEOUT));
    if (res < 0) {
        dprintf("serial::usart_shake PUT FAIL\n");
        return TRANSACTION_NO_RESPONSE;
    }

    // Which we always read back first so that we can error out correctly
    //   - due to the half duplex limitations on return codes, we always have to read *something*
    //   - without the read, write only transactions *always* succeed, even during the boot process where the slave is not ready
    res = uart_get(TIME_MS2I(TIMEOUT));
    if (res < 0) {
        dprintf("serial::usart_shake GET FAIL\n");
        return TRANSACTION_NO_RESPONSE;
    }

    uint8_t sstd_index_shake = (uint8_t)res;
    if (sstd_index_shake != (sstd_index ^ HANDSHAKE_MAGIC)) {
        dprintf("serial::usart_shake NO_RESPONSE\n");
        return TRANSACTION_NO_RESPONSE;
    }

    if (trans->initiator2target_buffer_size) {
        res = uart_send(trans->initiator2target_buffer, trans->initiator2target_buffer_size, TIME_MS2I(TIMEOUT));
        if (res < 0) {
            dprintf("serial::usart_transmit NO_RESPONSE\n");
            return TRANSACTION_NO_RESPONSE;
        }
    }

    if (trans->target2initiator_buffer_size) {
        res = uart_recv(trans->target2initiator_buffer, trans->target2initiator_buffer_size, TIME_MS2I(TIMEOUT));
        if (res < 0) {
            dprintf("serial::usart_receive NO_RESPONSE\n");
            return TRANSACTION_NO_RESPONSE;
        }
    }

    return TRANSACTION_END;
}
