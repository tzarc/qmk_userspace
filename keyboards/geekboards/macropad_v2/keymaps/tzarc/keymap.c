// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "tzarc.h"

enum {
    RE_1 = KEYMAP_SAFE_RANGE,
    RE_50,
};

// clang-format off
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    LAYOUT_ortho_2x4(
        QK_BOOT, RGB_MOD, RE_1, RE_50,
        KC_5,    KC_6, KC_7, KC_8
    )
};
// clang-format on

bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case RE_1:
            if (record->event.pressed) {
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            }
            break;
        case RE_50:
            if (record->event.pressed) {
                rgb_matrix_mode(RGB_MATRIX_CUSTOM_rv32_effect);
            }
            break;
    }
    return true;
}
