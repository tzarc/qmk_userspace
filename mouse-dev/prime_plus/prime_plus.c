// Copyright 2023 Dasky (@Dasky)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include <hal.h>
#include "color.h"

void board_init(void) {
    setPinOutput(A15);
    writePinHigh(A15); // Activate D+ pullup
    setPinInput(RGB_RED_PIN);
    setPinInput(RGB_GREEN_PIN);
    setPinInput(RGB_BLUE_PIN);
    AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
}

extern void ld7032_init(void);
extern void ld7032_test(void);

static PWMConfig pwmCFG = {
    .frequency = 0xFFFF,
    .period    = 256,
};

void rgb_set(uint8_t r, uint8_t g, uint8_t b) {
    if (r == 0) {
        pwmDisableChannel(&RGB_PWM_DRIVER, RGB_RED_PWM_CHANNEL - 1);
    } else {
        uint32_t duty = ((uint32_t)0xFFFF * r) / 0xFF;
        pwmEnableChannel(&RGB_PWM_DRIVER, RGB_RED_PWM_CHANNEL - 1, PWM_FRACTION_TO_WIDTH(&RGB_PWM_DRIVER, 0xFFFF, duty));
    }
    if (g == 0) {
        pwmDisableChannel(&RGB_PWM_DRIVER, RGB_GREEN_PWM_CHANNEL - 1);
    } else {
        uint32_t duty = ((uint32_t)0xFFFF * g) / 0xFF;
        pwmEnableChannel(&RGB_PWM_DRIVER, RGB_GREEN_PWM_CHANNEL - 1, PWM_FRACTION_TO_WIDTH(&RGB_PWM_DRIVER, 0xFFFF, duty));
    }
    if (b == 0) {
        pwmDisableChannel(&RGB_PWM_DRIVER, RGB_BLUE_PWM_CHANNEL - 1);
    } else {
        uint32_t duty = ((uint32_t)0xFFFF * b) / 0xFF;
        pwmEnableChannel(&RGB_PWM_DRIVER, RGB_BLUE_PWM_CHANNEL - 1, PWM_FRACTION_TO_WIDTH(&RGB_PWM_DRIVER, 0xFFFF, duty));
    }
}

void rgb_init(void) {
    palSetPadMode(PAL_PORT(RGB_RED_PIN), PAL_PAD(RGB_RED_PIN), PAL_MODE_ALTERNATE_PUSHPULL);
    palSetPadMode(PAL_PORT(RGB_GREEN_PIN), PAL_PAD(RGB_GREEN_PIN), PAL_MODE_ALTERNATE_PUSHPULL);
    palSetPadMode(PAL_PORT(RGB_BLUE_PIN), PAL_PAD(RGB_BLUE_PIN), PAL_MODE_ALTERNATE_PUSHPULL);
    pwmCFG.channels[RGB_RED_PWM_CHANNEL - 1].mode   = PWM_OUTPUT_ACTIVE_HIGH;
    pwmCFG.channels[RGB_GREEN_PWM_CHANNEL - 1].mode = PWM_OUTPUT_ACTIVE_HIGH;
    pwmCFG.channels[RGB_BLUE_PWM_CHANNEL - 1].mode  = PWM_OUTPUT_ACTIVE_HIGH;
    pwmStart(&RGB_PWM_DRIVER, &pwmCFG);
    rgb_set(0, 0, 0);
}

void keyboard_post_init_kb(void) {
    setPinOutput(OPTICAL_SW_PWR);
    writePinLow(OPTICAL_SW_PWR);

    ld7032_init();
    ld7032_test();

    rgb_init();
    rgb_set(RGB_TURQUOISE);

    keyboard_post_init_user();
}
