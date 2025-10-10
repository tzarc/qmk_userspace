// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "tzarc.h"
#include "tzarc_layout.h"

// clang-format off
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    LAYOUT_ortho_2x4(
        KC_GLOBE, RGB_NXT, RGB_PRV, QK_BOOT,
        KC_C,     KC_F,    KC_N,    KC_Q
    )
};
// clang-format on
