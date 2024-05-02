// Copyright 2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "tzarc_layout.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT(
        KC_MS_BTN1, KC_MS_BTN3, KC_MS_BTN2, // L, M, R
            LT(LAYER_RAISE, KC_MS_BTN5),    //    F
            LT(LAYER_LOWER, KC_MS_BTN4),    //    B
                    TIME_RESET              //   CPI
    ),
    [LAYER_LOWER] = LAYOUT(
        KC_TRNS,  KC_TRNS, KC_TRNS,
                  KC_TRNS,
                  KC_TRNS,
                  KC_TRNS
    ),
    [LAYER_RAISE] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS,
                 KC_TRNS,
                 KC_TRNS,
                 KC_TRNS
    ),
    [LAYER_ADJUST] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS,
                 KC_TRNS,
                 KC_TRNS,
                 KC_TRNS
    ),
};
// clang-format on

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [LAYER_BASE]   = {ENCODER_CCW_CW(KC_MS_WH_UP, KC_MS_WH_DOWN)},
    [LAYER_LOWER]  = {ENCODER_CCW_CW(RGB_HUD, RGB_HUI)},
    [LAYER_RAISE]  = {ENCODER_CCW_CW(RGB_MOD, RGB_RMOD)},
    [LAYER_ADJUST] = {ENCODER_CCW_CW(KC_NO, KC_NO)},
};
#endif

#include "pointing_device.h"

#ifdef MULTI_POINTING_ENABLE
const pointing_device_config_t pointing_device_configs[POINTING_DEVICE_COUNT] = {
    {.driver = &pmw3389_driver_spi_default, .comms_config = &pmw3389_config_spi_default, .throttle = 5, .motion = {.pin = B0, .active_low = true}},
};
#endif

void pointing_device_init_user(void) {
    pointing_device_set_cpi(6400);
}
