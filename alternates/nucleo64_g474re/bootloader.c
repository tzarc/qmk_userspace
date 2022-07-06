// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#include "bootloader.h"

#include "ch.h"
#include "hal.h"
#include "hal_pal.h"

#define SYMVAL(sym) (uint32_t)(((uint8_t*)&(sym)) - ((uint8_t*)0))
extern uint32_t __ram0_end__;
#define BOOTLOADER_MAGIC 0xDEADBEEF
#define MAGIC_ADDR (unsigned long*)(SYMVAL(__ram0_end__) - 4)

void bootloader_jump(void) {
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

        // STM32G474 has dual-bank flash, and we're incapable of jumping to the bootloader.
        // Instead, we do it with hardware... toggle PB0 low, let it charge a capacitor across BOOT0, then issue a system reset.
        palSetPadMode(GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL);
        palClearPad(GPIOB, 0);
        bootdelay(10000);
        palSetPadMode(GPIOB, 0, PAL_MODE_INPUT_ANALOG);
        palSetPad(GPIOB, 0);
        NVIC_SystemReset();
    }
}

void early_hardware_init_post(void) {
    enter_bootloader_mode_if_requested();
}
