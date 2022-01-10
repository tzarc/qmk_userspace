// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "tzarc.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_4x4_wrapper(
        ____________TZARC_4x4_MACROPAD_BASE_R1__________,
        ____________TZARC_4x4_MACROPAD_BASE_R2__________,
        ____________TZARC_4x4_MACROPAD_BASE_R3__________,
        ____________TZARC_4x4_MACROPAD_BASE_R4__________
    ),
    [1] = LAYOUT_ortho_4x4_wrapper(
        ____________TZARC_4x4_MACROPAD_A_R1_____________,
        ____________TZARC_4x4_MACROPAD_A_R2_____________,
        ____________TZARC_4x4_MACROPAD_A_R3_____________,
        ____________TZARC_4x4_MACROPAD_A_R4_____________
    ),
    [2] = LAYOUT_ortho_4x4_wrapper(
        ____________TZARC_4x4_MACROPAD_B_R1_____________,
        ____________TZARC_4x4_MACROPAD_B_R2_____________,
        ____________TZARC_4x4_MACROPAD_B_R3_____________,
        ____________TZARC_4x4_MACROPAD_B_R4_____________
    ),
    [3] = LAYOUT_ortho_4x4_wrapper(
        ____________TZARC_4x4_MACROPAD_C_R1_____________,
        ____________TZARC_4x4_MACROPAD_C_R2_____________,
        ____________TZARC_4x4_MACROPAD_C_R3_____________,
        ____________TZARC_4x4_MACROPAD_C_R4_____________
    ),
    [4] = LAYOUT_ortho_4x4_wrapper(
        ____________TZARC_4x4_MACROPAD_D_R1_____________,
        ____________TZARC_4x4_MACROPAD_D_R2_____________,
        ____________TZARC_4x4_MACROPAD_D_R3_____________,
        ____________TZARC_4x4_MACROPAD_D_R4_____________
    ),
};
// clang-format on

void keyboard_post_init_keymap(void) {
    debug_enable   = true;
    debug_keyboard = true;
}