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
#include <backlight.h>
#include <qp.h>

#include "serial_usart_statesync.h"

#include "gfx-djinn.c"
#include "gfx-lock_caps.c"
#include "gfx-lock_scrl.c"
#include "gfx-lock_num.c"

#define MEDIA_KEY_DELAY 2

enum { _QWERTY, _LOWER, _RAISE, _ADJUST };
#define KC_LWR MO(_LOWER)
#define KC_RSE MO(_RAISE)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] = LAYOUT_all(
        KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,    KC_GRV,                           KC_DEL,  KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,    KC_LBRC,                          KC_RBRC, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
        KC_LCTL,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,    KC_HOME,                          KC_PGUP, KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,    KC_END,                           KC_PGDN, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
                                   KC_LGUI, KC_LWR,  KC_SPC,  KC_NO,                            KC_NO,   KC_SPC,  KC_RSE,  KC_LALT,
                                                                      RGB_RMOD,         RGB_MOD,
                                                     KC_UP,                                              KC_UP,
                                            KC_LEFT, _______, KC_RIGHT,                         KC_LEFT, _______, KC_RIGHT,
                                                     KC_DOWN,                                            KC_DOWN
    ),
    [_LOWER] = LAYOUT_all(
        KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______,                         _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
        _______, _______, KC_UP,   _______, _______, _______, _______,                         _______, _______, _______, _______, _______, _______, _______,
        _______, KC_LEFT, KC_DOWN, KC_RIGHT,_______, _______, _______,                         _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,                         _______, _______, _______, _______, _______, _______, _______,
                                   _______, _______, _______, _______,                         _______, _______, _______, _______,
                                                                     BL_DEC,             BL_INC,
                                                     _______,                                           _______,
                                            _______, _______, _______,                         _______, _______, _______,
                                                     _______,                                           _______
    ),
    [_RAISE] = LAYOUT_all(
        KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______,                         _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
        _______,_______, KC_UP,    _______, _______, _______, _______,                         _______, _______, _______, _______, _______, _______, _______,
        _______,KC_LEFT, KC_DOWN, KC_RIGHT, _______, KC_UNDS, KC_NO,                           KC_NO,   KC_EQL,  _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, KC_MINS, KC_NO,                           KC_NO,   KC_PLUS, _______, _______, _______, _______, _______,
                                   _______, _______, _______, _______,                         _______, _______, _______, _______,
                                                                     _______,           _______,
                                                     _______,                                           _______,
                                            _______, _______, _______,                         _______, _______, _______,
                                                     _______,                                           _______
    ),
    [_ADJUST] = LAYOUT_all(
        _______, KC_CLCK, KC_NLCK, KC_SLCK, _______, _______, _______,                         _______, _______, _______, _______, DEBUG,   EEP_RST, RESET,
        _______, _______, _______, _______, _______, _______, _______,                         _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,                         _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,                         _______, _______, _______, _______, _______, _______, _______,
                                   _______, _______, _______, _______,                         _______, _______, _______, _______,
                                                                     _______,           _______,
                                                     _______,                                           _______,
                                            _______, _______, _______,                         _______, _______, _______,
                                                     _______,                                           _______
    )
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    // Default handler for lower/raise/adjust
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

void encoder_update_user(uint8_t index, bool clockwise) {
    uint8_t temp_mod = get_mods();
    uint8_t temp_osm = get_oneshot_mods();
    bool    is_ctrl  = (temp_mod | temp_osm) & MOD_MASK_CTRL;
    bool    is_shift = (temp_mod | temp_osm) & MOD_MASK_SHIFT;

    if (is_shift) {
        if (index == 0) { /* First encoder */
            if (clockwise) {
                rgblight_increase_hue();
            } else {
                rgblight_decrease_hue();
            }
        } else if (index == 1) { /* Second encoder */
            if (clockwise) {
                rgblight_decrease_sat();
            } else {
                rgblight_increase_sat();
            }
        }
    } else if (is_ctrl) {
        if (index == 0) { /* First encoder */
            if (clockwise) {
                rgblight_increase_val();
            } else {
                rgblight_decrease_val();
            }
        } else if (index == 1) { /* Second encoder */
            if (clockwise) {
                rgblight_increase_speed();
            } else {
                rgblight_decrease_speed();
            }
        }
    } else {
        if (index == 0) { /* First encoder */
            if (clockwise) {
                tap_code16(KC_MS_WH_DOWN);
            } else {
                tap_code16(KC_MS_WH_UP);
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
    }
}

void housekeeping_task_user(void) {
    kb_runtime_config*   kb_state   = get_split_sync_state_kb();
    user_runtime_config* user_state = get_split_sync_state_user();

    // If we're the master side, then propagate our runtime config to the slave
    if (is_keyboard_master()) {
        // Sync the LED state
        user_state->values.led_state = host_keyboard_led_state();
    }

    // Ensure state is sync'ed from master to slave, if required
    split_sync_user();

    if (kb_state->values.lcd_power) {
        bool            redraw_required = false;
        static uint16_t last_hue        = 0xFFFF;
        uint8_t         curr_hue        = rgblight_get_hue();
        if (last_hue != curr_hue) {
            redraw_required = true;
        }

        if (redraw_required) {
            last_hue = curr_hue;
            qp_drawimage_recolor(lcd, 120 - gfx_djinn->width / 2, 32, gfx_djinn, curr_hue, 255, 255);
            qp_rect(lcd, 0, 0, 8, 319, curr_hue, 255, 255, true);
            qp_rect(lcd, 231, 0, 239, 319, curr_hue, 255, 255, true);
        }

        static led_t last_led_state = {0};
        if (redraw_required || last_led_state.raw != user_state->values.led_state.raw) {
            last_led_state.raw = user_state->values.led_state.raw;
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 3), 0, gfx_lock_caps, curr_hue, 255, last_led_state.caps_lock ? 255 : 32);
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 2), 0, gfx_lock_num, curr_hue, 255, last_led_state.num_lock ? 255 : 32);
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 1), 0, gfx_lock_scrl, curr_hue, 255, last_led_state.scroll_lock ? 255 : 32);
        }
    }
}
