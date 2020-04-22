/* Copyright 2019 COSEYFANNITUTTI
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,  either version 2 of the License,  or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not,  see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include "tzarc.h"

enum { _BASE, _FUNC1 };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base keymaps
#define TABFN LT(_FUNC1, KC_TAB)
#define APPFN LT(_FUNC1, KC_APP)
#define CTLESC MT(MOD_LCTL, KC_ESC)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = LAYOUT_tkl_ansi(
      KC_ESC,             KC_F1,     KC_F2,       KC_F3,      KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,    KC_F9,    KC_F10,    KC_F11,   KC_F12,    KC_PSCR,   KC_SLCK,  KC_PAUS,
      KC_GRV,   KC_1,     KC_2,      KC_3,        KC_4,       KC_5,    KC_6,    KC_7,    KC_8,    KC_9,     KC_0,     KC_MINS,   KC_EQL,   KC_BSPC,   KC_INS,    KC_HOME,  KC_PGUP,
      TABFN,    KC_Q,     KC_W,      KC_E,        KC_R,       KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,     KC_P,     KC_LBRC,   KC_RBRC,  KC_BSLS,   KC_DEL,    KC_END,   KC_PGDN,
      CTLESC,   KC_A,     KC_S,      KC_D,        KC_F,       KC_G,    KC_H,    KC_J,    KC_K,    KC_L,     KC_SCLN,  KC_QUOT,   KC_ENT,
      KC_LSFT,            KC_Z,      KC_X,        KC_C,       KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,  KC_DOT,   KC_SLSH,   KC_RSFT,                        KC_UP,
      KC_LCTL,  KC_LGUI,  KC_LALT,                                     KC_SPC,                    KC_RALT,  KC_RGUI,  APPFN,     KC_RCTL,              KC_LEFT,  KC_DOWN,  KC_RGHT),

  [_FUNC1] = LAYOUT_tkl_ansi(
      KC_NOMODE,          KC_CONFIG, KC_WOWMODE,  KC_D3MODE,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_WIDE, KC_SCRIPT,  KC_BLOCKS,  KC_TRNS,   KC_TRNS,   KC_TRNS,  DEBUG,
      KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,     KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS,    KC_TRNS,    KC_TRNS,   KC_TRNS,   KC_TRNS,  KC_TRNS,
      KC_TRNS,  KC_TRNS,  KC_TRNS,   TIME_EEPRST, TIME_RESET, KC_TRNS, KC_TRNS, KC_P7,   KC_P8,   KC_P9,    KC_TRNS, KC_PSLS,    KC_PAST,    KC_TRNS,   KC_TRNS,   KC_TRNS,  KC_TRNS,
      KC_CAPS,  KC_TRNS,  KC_TRNS,   KC_TRNS,     KC_TRNS,    KC_TRNS, KC_TRNS, KC_P4,   KC_P5,   KC_P6,    KC_TRNS, KC_TRNS,    KC_TRNS,
      KC_TRNS,            KC_TRNS,   KC_TRNS,     KC_TRNS,    KC_TRNS, KC_TRNS, KC_P0,   KC_P1,   KC_P2,    KC_P3,   KC_TRNS,    KC_PMNS,                          KC_VOLU,
      KC_TRNS,  KC_TRNS,  KC_TRNS,                                     KC_TRNS,                   KC_TRNS,  KC_TRNS, KC_TRNS,    KC_TRNS,                KC_TRNS,  KC_VOLD,  KC_TRNS),
};
// clang-format on

bool process_record_keymap(uint16_t keycode, keyrecord_t* record) {
    static uint32_t reset_key_timer  = 0;
    static uint32_t eeprst_key_timer = 0;
    switch (keycode) {
        case TIME_RESET:
            if (record->event.pressed) {
                reset_key_timer = timer_read32();
            } else {
                if (timer_elapsed32(reset_key_timer) >= 500) {
                    reset_keyboard();
                }
            }
            return false;

        case TIME_EEPRST:
            if (record->event.pressed) {
                eeprst_key_timer = timer_read32();
            } else {
                if (timer_elapsed32(eeprst_key_timer) >= 500) {
                    eeconfig_init();
                }
            }
            return false;
    }

    return true;
}
