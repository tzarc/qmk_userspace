// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "os_detection.h"

enum { CUS_BTN4 = QK_USER, CUS_BTN5 } custom_keycodes;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    //                  LB,             MB,             RB,             FB, BB
    //                  CPI
    [0] = LAYOUT(KC_MS_BTN1, KC_MS_BTN3, KC_MS_BTN2, CUS_BTN5, CUS_BTN4, QK_BOOT)};

#include "pointing_device.h"

#ifdef MULTI_POINTING_ENABLE
const pointing_device_config_t pointing_device_configs[POINTING_DEVICE_COUNT] = {
    {.driver = &pmw3389_driver_spi_default, .comms_config = &pmw3389_config_spi_default, .throttle = 5, .motion = {.pin = B0, .active_low = true}},
};
#endif

void pointing_device_init_kb(void) {
    pointing_device_set_cpi(6400);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CUS_BTN4:
            if (record->event.pressed) {
                register_code16(detected_host_os() == OS_IOS ? G(KC_LEFT_BRACKET) : KC_MS_BTN4);
            } else {
                unregister_code16(detected_host_os() == OS_IOS ? G(KC_LEFT_BRACKET) : KC_MS_BTN4);
            }
            return false;
        case CUS_BTN5:
            if (record->event.pressed) {
                register_code16(detected_host_os() == OS_IOS ? G(KC_RIGHT_BRACKET) : KC_MS_BTN5);
            } else {
                unregister_code16(detected_host_os() == OS_IOS ? G(KC_RIGHT_BRACKET) : KC_MS_BTN5);
            }
            return false;
    }
    return true;
}

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = {ENCODER_CCW_CW(KC_MS_WH_UP, KC_MS_WH_DOWN)},
};
#endif
