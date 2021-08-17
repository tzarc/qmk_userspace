/* Copyright 2018-2020 Nick Brassel (@tzarc)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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