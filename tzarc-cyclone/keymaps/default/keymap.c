/*
Copyright 2012,2013 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

// clang-format off
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_Cyclone(\
        KC_A,      KC_B,    KC_C,    KC_D,      KC_E,    KC_F,    KC_G,    KC_H,
        KC_I,      KC_J,    KC_K,    KC_L,      KC_M,    KC_N,    KC_O,    KC_P,
        KC_ESC,             KC_F1,   KC_F2,     KC_F3,   KC_F4,            KC_F5,
        KC_GRAVE,  KC_1,    KC_2,    KC_3,      KC_4,    KC_5,    KC_6,    KC_7,
        KC_TAB,    KC_Q,    KC_W,    KC_E,      KC_R,    KC_T,    KC_Y,
        KC_CAPS,   KC_A,    KC_S,    KC_D,      KC_F,    KC_G,    KC_H,       RGB_VAI,
        KC_LSHIFT, KC_Z,    KC_X,    KC_C,      KC_V,    KC_B,            RGB_HUI, RGB_MOD,
        KC_LCTRL,  KC_LGUI, KC_LALT,       KC_SPACE,          MO(1),          RGB_VAD
    ),
    [1] = LAYOUT_Cyclone(\
        RGB_TOG,   RGB_MOD, KC_NO,   KC_NO,     KC_NO,   DEBUG,   RESET,   EEP_RST,
        RGB_HUI,   RGB_HUD, RGB_SAI, RGB_SAD,   RGB_VAI, RGB_VAD, RGB_SPI, RGB_SPD,
        KC_NO,              KC_NO,   KC_NO,     KC_NO,   KC_NO,            KC_NO,
        KC_NO,     KC_NO,   KC_NO,   KC_NO,     KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,     KC_NO,   KC_NO,   KC_NO,     KC_NO,   KC_NO,   KC_NO,
        KC_NO,     KC_NO,   KC_NO,   KC_NO,     KC_NO,   KC_NO,   KC_NO,      KC_NO,
        KC_NO,     KC_NO,   KC_NO,   KC_NO,     KC_NO,   KC_NO,            KC_NO, KC_NO,
        KC_NO,     KC_NO,   KC_NO,        KC_NO,             KC_NO,           KC_NO
    )
};
// clang-format on

const uint16_t fn_actions[] = {};
