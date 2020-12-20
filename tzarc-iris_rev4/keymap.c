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

  [LAYER_BASE] = LAYOUT_wrapper(
     ____________TZARC_6x4_BASE_R1_L__________,                           ____________TZARC_6x4_BASE_R1_R__________,
     ____________TZARC_6x4_BASE_R2_L__________,                           ____________TZARC_6x4_BASE_R2_R__________,
     ____________TZARC_6x4_BASE_R3_L__________,                           ____________TZARC_6x4_BASE_R3_R__________,
     ____________TZARC_6x4_BASE_R4_L__________, KC_GRV,           KC_DEL, ____________TZARC_6x4_BASE_R4_R__________,
                             KC_LGUI, KC_LOWER, KC_SPC,           KC_SPC, KC_RAISE, KC_LALT
  ),

  [LAYER_LOWER] = LAYOUT_wrapper(
     ____________TZARC_6x4_LOWER_R1_L_________,                           ____________TZARC_6x4_LOWER_R1_R_________,
     ____________TZARC_6x4_LOWER_R2_L_________,                           ____________TZARC_6x4_LOWER_R2_R_________,
     ____________TZARC_6x4_LOWER_R3_L_________,                           ____________TZARC_6x4_LOWER_R3_R_________,
     ____________TZARC_6x4_LOWER_R4_L_________, _______,         KC_HOME, ____________TZARC_6x4_LOWER_R4_R_________,
                              _______, _______, KC_APP,           KC_END, _______, _______
  ),

  [LAYER_RAISE] = LAYOUT_wrapper(
     ____________TZARC_6x4_RAISE_R1_L_________,                           ____________TZARC_6x4_RAISE_R1_R_________,
     ____________TZARC_6x4_RAISE_R2_L_________,                           ____________TZARC_6x4_RAISE_R2_R_________,
     ____________TZARC_6x4_RAISE_R3_L_________,                           ____________TZARC_6x4_RAISE_R3_R_________,
     ____________TZARC_6x4_RAISE_R4_L_________, KC_PGUP,          KC_BRK, ____________TZARC_6x4_RAISE_R4_R_________,
                              _______, _______, KC_PGDN,         KC_PSCR, _______, _______
  ),

  [LAYER_ADJUST] = LAYOUT_wrapper(
     ____________TZARC_6x4_ADJUST_R1_L________,                           ____________TZARC_6x4_ADJUST_R1_R________,
     ____________TZARC_6x4_ADJUST_R2_L________,                           ____________TZARC_6x4_ADJUST_R2_R________,
     ____________TZARC_6x4_ADJUST_R3_L________,                           ____________TZARC_6x4_ADJUST_R3_R________,
     ____________TZARC_6x4_ADJUST_R4_L________, _______,         _______, ____________TZARC_6x4_ADJUST_R4_R________,
                              _______, _______, _______,         _______, _______, _______
  ),
};
// clang-format on

void eeconfig_init_keymap(void) {
    breathing_disable();
    rgblight_mode(RGBLIGHT_MODE_STATIC_LIGHT);
    rgblight_sethsv(58, 255, 80);
    backlight_enable();
    backlight_level(BACKLIGHT_LEVELS);
}
