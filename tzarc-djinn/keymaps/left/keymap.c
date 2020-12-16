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

#define MEDIA_KEY_DELAY 2

enum { _QWERTY, _LOWER, _RAISE, _ADJUST };
#define KC_LWR MO(_LOWER)
#define KC_RSE MO(_RAISE)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] = LAYOUT_leftonly(
        KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,    KC_GRV,
        KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,    KC_LBRC,
        KC_LCTL,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,    KC_HOME,
        KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,    KC_END,
                                   KC_LGUI, KC_LWR,  KC_SPC,  KC_NO,
                                                                      RGB_RMOD,
                                                     KC_UP,
                                            KC_LEFT, KC_MUTE, KC_RIGHT,
                                                     KC_DOWN
    ),
    [_LOWER] = LAYOUT_leftonly(
        KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______,
        _______, _______, KC_UP,   _______, _______, _______, _______,
        _______, KC_LEFT, KC_DOWN, KC_RIGHT,_______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,
                                   _______, _______, _______, _______,
                                                                     BL_DEC,
                                                     _______,
                                            _______, _______, _______,
                                                     _______
    ),
    [_RAISE] = LAYOUT_leftonly(
        KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______,
        _______,_______, KC_UP,    _______, _______, _______, _______,
        _______,KC_LEFT, KC_DOWN, KC_RIGHT, _______, KC_UNDS, KC_NO,
        _______, _______, _______, _______, _______, KC_MINS, KC_NO,
                                   _______, _______, _______, _______,
                                                                     _______,
                                                     _______,
                                            _______, _______, _______,
                                                     _______
    ),
    [_ADJUST] = LAYOUT_leftonly(
        _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,
                                   _______, _______, _______, _______,
                                                                     _______,
                                                     _______,
                                            _______, _______, _______,
                                                     _______
    )
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    // Default handler for lower/raise/adjust
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

void encoder_update_user(uint8_t index, bool clockwise) {
    uint8_t temp_mod   = get_mods();
    uint8_t temp_osm   = get_oneshot_mods();
    bool    is_shifted = (temp_mod | temp_osm) & MOD_MASK_SHIFT;

    if (!is_shifted) {
        if (index == 0) { /* First encoder */
            if (clockwise) {
                rgblight_increase_hue_noeeprom();
            } else {
                rgblight_decrease_hue_noeeprom();
            }
        } else if (index == 1) { /* Second encoder */
            uint16_t held_keycode_timer = timer_read();
            uint16_t mapped_code        = 0;
            if (clockwise) {
                mapped_code = KC_VOLD;
            } else {
                mapped_code = KC_VOLU;
            }
            register_code(mapped_code);
            while (timer_elapsed(held_keycode_timer) < MEDIA_KEY_DELAY)
                ; /* no-op */
            unregister_code(mapped_code);
        }
    } else {
        if (index == 0) { /* First encoder */
            if (clockwise) {
                rgblight_increase_val_noeeprom();
            } else {
                rgblight_decrease_val_noeeprom();
            }
        } else if (index == 1) { /* Second encoder */
            if (clockwise) {
                rgblight_decrease_sat_noeeprom();
            } else {
                rgblight_increase_sat_noeeprom();
            }
        }
    }
}