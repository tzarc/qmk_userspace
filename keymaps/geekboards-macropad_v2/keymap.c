// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "tzarc.h"

// clang-format off
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    LAYOUT_ortho_2x4(
        QK_BOOT, KC_2, KC_3, KC_4,
        KC_5,    KC_6, KC_7, KC_8
    )
};
// clang-format on
