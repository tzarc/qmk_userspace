#include "bootloader.h"

#include "ch.h"
#include "hal.h"
#include "hal_pal.h"

#define SYMVAL(sym) (uint32_t)(((uint8_t*)&(sym)) - ((uint8_t*)0))
extern uint32_t __ram0_end__;
#define BOOTLOADER_MAGIC 0xDEADBEEF
#define MAGIC_ADDR (unsigned long*)(SYMVAL(__ram0_end__) - 4)

void bootloader_jump(void) {
#ifdef OLED_DRIVER_ENABLE
    extern bool oled_off(void);
    oled_off();
#endif  // OLED_DRIVER_ENABLE

    *MAGIC_ADDR = BOOTLOADER_MAGIC;
    NVIC_SystemReset();
}

#define bootdelay(loopcount)                  \
    do {                                      \
        for (int i = 0; i < loopcount; ++i) { \
            __asm__ volatile("nop\n\t"        \
                             "nop\n\t"        \
                             "nop\n\t");      \
        }                                     \
    } while (0)

void enter_bootloader_mode_if_requested(void) {
    unsigned long* check = MAGIC_ADDR;
    if (*check == BOOTLOADER_MAGIC) {
        *check = 0;

        // STM32L072/082 has dual-bank flash, and we're incapable of jumping to the bootloader as a result. The first
        // valid flash bank is executed unconditionally after a reset, so it doesn't enter DFU unless BOOT0 is high.
        // Instead, we do it with hardware...in this case, we pull PB0 low, which is connected to a P-channel transistor
        // which connects 3.3V to BOOT0's RC charging circuit, let it charge the capacitor, and issue a system reset.
        palSetPadMode(GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL);
        palClearPad(GPIOB, 0);
        bootdelay(10000);
        palSetPadMode(GPIOB, 0, PAL_MODE_INPUT_ANALOG);
        palSetPad(GPIOB, 0);
        NVIC_SystemReset();
    }
}
