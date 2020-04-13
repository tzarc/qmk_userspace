#include "debug.h"
#include "print.h"

#include "cyclone.h"

#include "ch.h"
#include "chtrace.h"
#include "chvt.h"
#include "hal.h"
#include "hal_pal.h"

// #define TEST_AUDIO_PIN_OUTPUT
// #define TEST_SPI_OUTPUT

#ifdef OLED_DRIVER_ENABLE
#    include "oled_driver.h"
#endif  // OLED_DRIVER_ENABLE

#ifdef QWIIC_MICRO_OLED_ENABLE
#    include "micro_oled.h"
static void draw_ui(void);
#endif  // QWIIC_MICRO_OLED_ENABLE

#ifdef RGB_MATRIX_ENABLE
led_config_t g_led_config = {{// Key Matrix to LED Index
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0}},
                             {// LED Index to Physical Position
                              {224, 64},
                              {208, 64},
                              {192, 64},
                              {176, 64},
                              {160, 64},
                              {144, 64},
                              {128, 64},
                              {112, 64},
                              {96, 64},
                              {80, 64},
                              {64, 64},
                              {48, 64},
                              {32, 64},
                              {16, 64},
                              {0, 64}},
                             {// LED Index to Flag
                              4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4}};
#endif  // RGB_MATRIX_ENABLE

uint8_t prng(void) {
    static uint8_t s = 0xAA, a = 0;
    s ^= s << 3;
    s ^= s >> 5;
    s ^= a++ >> 2;
    return s;
}

void matrix_io_delay(void) { __asm__ volatile("nop\nnop\nnop\n"); }

void matrix_scan_user(void) {
#ifdef TEST_SPI_OUTPUT
    static const SPIConfig hs_spicfg = {0, NULL, GPIOA, 1, SPI_CR1_BR_0, 0};

    static uint32_t last = 0;
    uint32_t        now  = timer_read32();
    if (now - last > 20) {
        const char txbuf[4] = {0x12, 0x34, 0x56, 0x78};
        char       rxbuf[4] = {0};
        last                = now;

        spiStart(&SPID1, &hs_spicfg);         /* Setup transfer parameters.       */
        spiSelect(&SPID1);                    /* Slave Select assertion.          */
        spiExchange(&SPID1, 4, txbuf, rxbuf); /* Atomic transfer operations.      */
        spiUnselect(&SPID1);                  /* Slave Select de-assertion.       */
        spiStop(&SPID1);
    }
#endif

#ifdef QWIIC_MICRO_OLED_ENABLE
    (void)draw_ui;
    draw_ui();
#endif  // QWIIC_MICRO_OLED_ENABLE

    /*
    static uint32_t        last_oled_timer_print = 0;
    extern uint32_t oled_timeout;
    uint32_t now = timer_read32();
    if (now - last_oled_timer_print > 1000) {
        last_oled_timer_print = now;
        uprintf("%6u/%6u/%6u\n", now, oled_timeout, oled_timeout - now);
    }
    */

    static uint32_t        last_eeprom_access = 0;
    uint32_t now = timer_read32();
    if (now - last_eeprom_access > 5000) {
        dprint("reading eeprom\n");
        last_eeprom_access = now;

        union {
            uint8_t bytes[4];
            uint32_t raw;
        } tmp;
        tmp.bytes[0] = prng();
        tmp.bytes[1] = prng();
        tmp.bytes[2] = prng();
        tmp.bytes[3] = prng();

        eeconfig_update_user(tmp.raw);
        eeconfig_read_user();
    }
}

void matrix_init_kb(void) {
#ifdef TEST_SPI_OUTPUT
    palSetPadMode(GPIOB, 3, PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST); /* New SCK.     */
    palSetPadMode(GPIOB, 4, PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST); /* New MISO.    */
    palSetPadMode(GPIOB, 5, PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST); /* New MOSI.    */
    palSetPad(GPIOA, 1);
    palSetPadMode(GPIOA, 1, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST); /* New CS.      */
#endif

    palSetPadMode(GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOB, 0);

    // Set the level shifter GPIO directions and initial states, needs to be before shift registers get initialised
    setPinOutput(PIN_EN_5V_IO_INV);
    setPinOutput(PIN_EN_5V_IO);

    // Enable the 3V3 -> 5V transceivers
    writePinLow(PIN_EN_5V_IO_INV);
    writePinHigh(PIN_EN_5V_IO);

    // Run the rest of the init routines
    matrix_init_user();
}

void keyboard_post_init_kb(void) {
#ifdef EEPROM_ENABLE
    if (!eeconfig_is_enabled()) {
        eeconfig_init();
    }
#endif  // EEPROM_ENABLE

    debug_enable   = true;
    debug_matrix   = true;
    debug_keyboard = false;
    debug_mouse    = false;

#ifdef TEST_AUDIO_PIN_OUTPUT
    setPinOutput(A4);
    setPinOutput(A5);
    for (uint32_t i = 0; i < 2000; ++i) {
        writePinHigh(A4);
        writePinLow(A5);
        wait_ms(1);
        writePinLow(A4);
        writePinHigh(A5);
        wait_ms(1);
    }
#endif  // TEST_AUDIO_PIN_OUTPUT
}

#ifdef OLED_DRIVER_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    // We're vertical...
    return OLED_ROTATION_270;
}

uint8_t last_led_usb_state = 0xFF;
void    oled_task_user(void) {
    // Host Keyboard LED Status
    uint8_t led_usb_state = host_keyboard_leds();
    if (last_led_usb_state != led_usb_state) {
        last_led_usb_state = led_usb_state;
        oled_write_ln_P(led_usb_state & (1 << USB_LED_NUM_LOCK) ? PSTR("NUM  ") : PSTR("     "), false);
        oled_advance_page(true);
        oled_write_ln_P(led_usb_state & (1 << USB_LED_CAPS_LOCK) ? PSTR("CAP  ") : PSTR("     "), false);
        oled_advance_page(true);
        oled_write_ln_P(led_usb_state & (1 << USB_LED_SCROLL_LOCK) ? PSTR("SCR  ") : PSTR("     "), false);
    }
}
#endif  // OLED_DRIVER_ENABLE

#ifdef QWIIC_MICRO_OLED_ENABLE
uint8_t last_led_usb_state = 0xFF;
void    draw_ui(void) {
    uint8_t led_usb_state = host_keyboard_leds();
    if (last_led_usb_state != led_usb_state) {
        last_led_usb_state = led_usb_state;
        draw_string(2, 2, led_usb_state & (1 << USB_LED_NUM_LOCK) ? PSTR("NUM  ") : PSTR("     "), PIXEL_ON, NORM, 0);
        draw_string(2, 12, led_usb_state & (1 << USB_LED_CAPS_LOCK) ? PSTR("CAPS") : PSTR("     "), PIXEL_ON, NORM, 0);
        draw_string(2, 22, led_usb_state & (1 << USB_LED_SCROLL_LOCK) ? PSTR("SCRL") : PSTR("     "), PIXEL_ON, NORM, 0);
    }
    send_buffer();
}
#endif  // QWIIC_MICRO_OLED_ENABLE

void chibi_system_halt_hook(const char* reason) {
    // re-route to QMK toolbox...
    uprintf("system halting: %s\n", reason);
}

void chibi_system_trace_hook(void* tep) {
    // re-route to QMK toolbox...
    uprintf("trace\n");
}

void chibi_debug_check_hook(const char* func, const char* condition, int value) {
    // re-route to QMK toolbox...
    uprintf("%s debug check failure: (%s) == %s\n", func, condition, value ? "true" : "false");
    // ...and hard-loop fail
    while (1) {
    }
}

void chibi_debug_assert_hook(const char* func, const char* condition, int value, const char* reason) {
    // re-route to QMK toolbox...
    uprintf("%s debug assert (%s) failure: (%s) == %s\n", func, reason, condition, value ? "true" : "false");
    // ...and hard-loop fail
    while (1) {
    }
}
