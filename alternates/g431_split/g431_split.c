// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <quantum.h>

void keyboard_post_init_user(void) {
    // Customise these values to desired behaviour
    debug_enable = true;
    debug_matrix = true;
    // debug_keyboard=true;
    // debug_mouse=true;
}

void matrix_io_delay(void) {
    __asm__ volatile("nop\nnop\nnop\n");
}

#include "hal.h"

#define FLASH_KEY1 0x45670123
#define FLASH_KEY2 0xCDEF89AB

#define FLASH_OPTKEY1 0x08192A3B
#define FLASH_OPTKEY2 0x4C5D6E7F

static inline void OPT_WaitNotBusy(void) {
    uint32_t sr = 0;
    for (sr = FLASH->SR; sr & FLASH_SR_BSY; sr = FLASH->SR) {
        __WFI();
    }
}

static inline void OPT_Unlock(void) {
    OPT_WaitNotBusy();
    if (FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
    if (FLASH->CR & FLASH_CR_OPTLOCK) {
        FLASH->OPTKEYR = FLASH_OPTKEY1;
        FLASH->OPTKEYR = FLASH_OPTKEY2;
    }
}

static inline void OPT_Lock(void) {
    OPT_WaitNotBusy();
    FLASH->CR |= FLASH_CR_OPTLOCK;
    FLASH->CR |= FLASH_CR_LOCK;
}

static inline void OPT_Set(uint32_t OptionBytes) {
    __IO uint32_t *optionBytes = &(FLASH->OPTR);
    if (*optionBytes != OptionBytes) {
        OPT_Unlock();
        *optionBytes = OptionBytes;
        FLASH->CR |= FLASH_CR_OPTSTRT;
        OPT_Lock();
        NVIC_SystemReset();
    }
}

void early_hardware_init_pre(void) {
    OPT_Set(0xFFEFF8AA);

#if EARLY_INIT_PERFORM_BOOTLOADER_JUMP
    void enter_bootloader_mode_if_requested(void);
    enter_bootloader_mode_if_requested();
#endif // EARLY_INIT_PERFORM_BOOTLOADER_JUMP
}
