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

void enter_bootloader_mode_if_requested(void) {
    unsigned long* check = MAGIC_ADDR;
    if (*check == BOOTLOADER_MAGIC) {
        *check = 0;

        // STM32L082 has dual-bank flash, and we're incapable of jumping to the bootloader.
        // Instead, we do it with hardware... toggle PB0 high, let it charge a capacitor across BOOT0, then issue a system reset.
        palSetPadMode(GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL);
        palSetPad(GPIOB, 0);
        chThdSleepMilliseconds(100);
        palSetPadMode(GPIOB, 0, PAL_MODE_INPUT_PULLDOWN);
        palClearPad(GPIOB, 0);
        chThdSleepMilliseconds(1);
        NVIC_SystemReset();
    }
}
