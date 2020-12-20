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

#include "tzarc.h"
#include "serial_usart_dataxfer.h"

#include "gfx-djinn.c"
#include "gfx-lock_caps.c"
#include "gfx-lock_scrl.c"
#include "gfx-lock_num.c"

#define MEDIA_KEY_DELAY 2

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT_all_wrapper(
        ____________TZARC_7x4_BASE_R1_L__________,                                            ____________TZARC_7x4_BASE_R1_R__________,
        ____________TZARC_7x4_BASE_R2_L__________,                                            ____________TZARC_7x4_BASE_R2_R__________,
        ____________TZARC_7x4_BASE_R3_L__________,                                            ____________TZARC_7x4_BASE_R3_R__________,
        ____________TZARC_7x4_BASE_R4_L__________,                                            ____________TZARC_7x4_BASE_R4_R__________,
                     KC_LGUI, KC_LOWER,  KC_SPC,  KC_NO,                                 KC_NO,   KC_SPC,  KC_RAISE,  KC_LALT,
                                                           RGB_RMOD,          RGB_MOD,
                                KC_UP,                                                                 KC_UP,
                       KC_LEFT, KC_MUTE, KC_RIGHT,                                            KC_LEFT, KC_MUTE, KC_RIGHT,
                                KC_DOWN,                                                               KC_DOWN
    ),
    [LAYER_LOWER] = LAYOUT_all_wrapper(
        ____________TZARC_7x4_LOWER_R1_L_________,                                            ____________TZARC_7x4_LOWER_R1_R_________,
        ____________TZARC_7x4_LOWER_R2_L_________,                                            ____________TZARC_7x4_LOWER_R2_R_________,
        ____________TZARC_7x4_LOWER_R3_L_________,                                            ____________TZARC_7x4_LOWER_R3_R_________,
        ____________TZARC_7x4_LOWER_R4_L_________,                                            ____________TZARC_7x4_LOWER_R4_R_________,
                     _______, _______, _______, _______,                                   _______, _______, _______, _______,
                                                           BL_DEC,            BL_INC,
                                _______,                                                             _______,
                       _______, _______, _______,                                           _______, _______, _______,
                                _______,                                                             _______
    ),
    [LAYER_RAISE] = LAYOUT_all_wrapper(
        ____________TZARC_7x4_RAISE_R1_L_________,                                            ____________TZARC_7x4_RAISE_R1_R_________,
        ____________TZARC_7x4_RAISE_R2_L_________,                                            ____________TZARC_7x4_RAISE_R2_R_________,
        ____________TZARC_7x4_RAISE_R3_L_________,                                            ____________TZARC_7x4_RAISE_R3_R_________,
        ____________TZARC_7x4_RAISE_R4_L_________,                                            ____________TZARC_7x4_RAISE_R4_R_________,
                     _______, _______, _______, _______,                                   _______, _______, _______, _______,
                                                           _______,           _______,
                                _______,                                                             _______,
                       _______, _______, _______,                                           _______, _______, _______,
                                _______,                                                             _______
    ),
    [LAYER_ADJUST] = LAYOUT_all_wrapper(
        ____________TZARC_7x4_ADJUST_R1_L________,                                            ____________TZARC_7x4_ADJUST_R1_R________,
        ____________TZARC_7x4_ADJUST_R2_L________,                                            ____________TZARC_7x4_ADJUST_R2_R________,
        ____________TZARC_7x4_ADJUST_R3_L________,                                            ____________TZARC_7x4_ADJUST_R3_R________,
        ____________TZARC_7x4_ADJUST_R4_L________,                                            ____________TZARC_7x4_ADJUST_R4_R________,
                     _______, _______, _______, _______,                                   _______, _______, _______, _______,
                                                           _______,           _______,
                                _______,                                                             _______,
                       _______, _______, _______,                                           _______, _______, _______,
                                _______,                                                             _______
    )
};
// clang-format on

void eeconfig_init_keymap(void) {
    rgblight_mode(RGBLIGHT_MODE_STATIC_LIGHT);
    rgblight_sethsv(128, 255, 255);
    backlight_enable();
    backlight_level(BACKLIGHT_LEVELS);
}

void encoder_update_keymap(uint8_t index, bool clockwise) {
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

#pragma pack(push)
#pragma pack(1)
typedef union user_runtime_config {
    struct {
        led_t led_state;
    } values;
    uint8_t raw;
} user_runtime_config;
#pragma pack(pop)
_Static_assert(sizeof(user_runtime_config) == 1, "Invalid data transfer size for user runtime data");

static user_runtime_config user_conf;

bool serial_dataxfer_receive_user(const void* data, size_t len) {
    const user_runtime_config* xfer = (const user_runtime_config*)data;
    user_conf.raw                   = xfer->raw;
    serial_dataxfer_respond_user(&user_conf, sizeof(user_conf));
    return true;
}

void housekeeping_task_keymap(void) {
    if (is_keyboard_master()) {
        static user_runtime_config last_data;
        uint32_t                   now = timer_read32();

        // Sync the LED state
        user_conf.values.led_state = host_keyboard_led_state();

        // Send the data from the master to the slave
        static uint32_t last_sync = 0;
        if (now - last_sync > 250 || last_data.raw != user_conf.raw) {  // At worst, resync every 250ms
            last_sync     = now;
            last_data.raw = user_conf.raw;

            user_runtime_config slave_runtime_cfg;
            slave_runtime_cfg.raw = 0;
            serial_dataxfer_transaction_user(&user_conf, sizeof(user_conf), &slave_runtime_cfg, sizeof(slave_runtime_cfg));
        }
    }

    if (kb_conf.values.lcd_power) {
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
        if (redraw_required || last_led_state.raw != user_conf.values.led_state.raw) {
            last_led_state.raw = user_conf.values.led_state.raw;
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 3), 0, gfx_lock_caps, curr_hue, 255, last_led_state.caps_lock ? 255 : 32);
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 2), 0, gfx_lock_num, curr_hue, 255, last_led_state.num_lock ? 255 : 32);
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 1), 0, gfx_lock_scrl, curr_hue, 255, last_led_state.scroll_lock ? 255 : 32);
        }
    }
}
