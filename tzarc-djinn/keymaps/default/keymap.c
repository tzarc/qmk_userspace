/* Copyright 2021 Nick Brassel (@tzarc)
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
#include <hal.h>
#include <string.h>
#include <ctype.h>
#include <backlight.h>
#include <qp.h>
#include <printf.h>
#include <transactions.h>
#include <split_util.h>

#include "graphics/src/djinn.c"
#include "graphics/src/lock-caps-ON.c"
#include "graphics/src/lock-scrl-ON.c"
#include "graphics/src/lock-num-ON.c"
#include "graphics/src/lock-caps-OFF.c"
#include "graphics/src/lock-scrl-OFF.c"
#include "graphics/src/lock-num-OFF.c"
#include "graphics/src/thintel15.c"

#define MEDIA_KEY_DELAY 5

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

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [LAYER_BASE]   = {ENCODER_CCW_CW(KC_MS_WH_UP, KC_MS_WH_DOWN), ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [LAYER_LOWER]  = {ENCODER_CCW_CW(RGB_HUD, RGB_HUI), ENCODER_CCW_CW(RGB_SAD, RGB_SAI)},
    [LAYER_RAISE]  = {ENCODER_CCW_CW(RGB_VAD, RGB_VAI), ENCODER_CCW_CW(RGB_SPD, RGB_SPI)},
    [LAYER_ADJUST] = {ENCODER_CCW_CW(RGB_RMOD, RGB_MOD), ENCODER_CCW_CW(KC_LEFT, KC_RIGHT)},
};
#else
bool encoder_update_user(uint8_t index, bool clockwise) {
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
            if (clockwise) {
                tap_code_delay(KC_VOLU, MEDIA_KEY_DELAY);
            } else {
                tap_code_delay(KC_VOLD, MEDIA_KEY_DELAY);
            }
        }
    }
    return false;
}
#endif

#ifdef SWAP_HANDS_ENABLE
const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
    {{6, 6}, {5, 6}, {4, 6}, {3, 6}, {2, 6}, {1, 6}, {0, 6}}, {{6, 7}, {5, 7}, {4, 7}, {3, 7}, {2, 7}, {1, 7}, {0, 7}}, {{6, 8}, {5, 8}, {4, 8}, {3, 8}, {2, 8}, {1, 8}, {0, 8}}, {{6, 9}, {5, 9}, {4, 9}, {3, 9}, {2, 9}, {1, 9}, {0, 9}}, {{0, 0}, {0, 0}, {0, 0}, {6, 10}, {5, 10}, {4, 10}, {3, 10}}, {{0, 0}, {6, 11}, {5, 11}, {4, 11}, {3, 11}, {2, 11}, {1, 11}},

    {{6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0}, {0, 0}}, {{6, 1}, {5, 1}, {4, 1}, {3, 1}, {2, 1}, {1, 1}, {0, 1}}, {{6, 2}, {5, 2}, {4, 2}, {3, 2}, {2, 2}, {1, 2}, {0, 2}}, {{6, 3}, {5, 3}, {4, 3}, {3, 3}, {2, 3}, {1, 3}, {0, 3}}, {{0, 0}, {0, 0}, {0, 0}, {6, 4}, {5, 4}, {4, 4}, {3, 4}},     {{0, 0}, {6, 5}, {5, 5}, {4, 5}, {3, 5}, {2, 5}, {1, 5}},
};
#    ifdef ENCODER_MAP_ENABLE
const uint8_t PROGMEM encoder_hand_swap_config[NUM_ENCODERS] = {1, 0};
#    endif
#endif

// clang-format on

//----------------------------------------------------------
// RGB Matrix naming
#if defined(RGB_MATRIX_ENABLE)
#    include <rgb_matrix.h>

#    if defined(RGB_MATRIX_EFFECT)
#        undef RGB_MATRIX_EFFECT
#    endif  // defined(RGB_MATRIX_EFFECT)

#    define RGB_MATRIX_EFFECT(x) RGB_MATRIX_EFFECT_##x,
enum {
    RGB_MATRIX_EFFECT_NONE,
#    include "rgb_matrix_effects.inc"
#    undef RGB_MATRIX_EFFECT
#    ifdef RGB_MATRIX_CUSTOM_KB
#        include "rgb_matrix_kb.inc"
#    endif
#    ifdef RGB_MATRIX_CUSTOM_USER
#        include "rgb_matrix_user.inc"
#    endif
};

#    define RGB_MATRIX_EFFECT(x)    \
        case RGB_MATRIX_EFFECT_##x: \
            return #x;
const char *rgb_matrix_name(uint8_t effect) {
    switch (effect) {
        case RGB_MATRIX_EFFECT_NONE:
            return "NONE";
#    include "rgb_matrix_effects.inc"
#    undef RGB_MATRIX_EFFECT
#    ifdef RGB_MATRIX_CUSTOM_KB
#        include "rgb_matrix_kb.inc"
#    endif
#    ifdef RGB_MATRIX_CUSTOM_USER
#        include "rgb_matrix_user.inc"
#    endif
        default:
            return "UNKNOWN";
    }
}
#endif  // defined(RGB_MATRIX_ENABLE)

//----------------------------------------------------------
// Sync
#pragma pack(push)
#pragma pack(1)

typedef struct user_runtime_config {
    uint32_t scan_rate;
} user_runtime_config;

#pragma pack(pop)

_Static_assert(sizeof(user_runtime_config) == 4, "Invalid data transfer size for user sync data");

user_runtime_config user_state;

void rpc_user_sync_callback(uint8_t initiator2target_buffer_size, const void *initiator2target_buffer, uint8_t target2initiator_buffer_size, void *target2initiator_buffer) {
    if (initiator2target_buffer_size == sizeof(user_state)) {
        memcpy(&user_state, initiator2target_buffer, sizeof(user_runtime_config));
    }
}

void keyboard_post_init_user(void) {
    // Register keyboard state sync split transaction
    transaction_register_rpc(USER_DATA_SYNC, rpc_user_sync_callback);

    // Reset the initial shared data value between master and slave
    memset(&user_state, 0, sizeof(user_state));
}

void user_state_update(void) {
    if (is_keyboard_master()) {
        // Keep the scan rate in sync
        user_state.scan_rate = get_matrix_scan_rate();
    }
}

void user_state_sync(void) {
    if (!is_transport_connected()) return;

    if (is_keyboard_master()) {
        // Keep track of the last state, so that we can tell if we need to propagate to slave
        static user_runtime_config last_user_state;
        static uint32_t            last_sync;
        bool                       needs_sync = false;

        // Check if the state values are different
        if (memcmp(&user_state, &last_user_state, sizeof(user_runtime_config))) {
            needs_sync = true;
            memcpy(&last_user_state, &user_state, sizeof(user_runtime_config));
        }

        // Send to slave every 125ms regardless of state change
        if (timer_elapsed32(last_sync) > 125) {
            needs_sync = true;
        }

        // Perform the sync if requested
        if (needs_sync) {
            if (transaction_rpc_send(USER_DATA_SYNC, sizeof(user_runtime_config), &user_state)) {
                last_sync = timer_read32();
            }
        }
    }
}

void housekeeping_task_user(void) {
    // Update kb_state so we can send to slave
    user_state_update();

    // Data sync from master to slave
    user_state_sync();
}

//----------------------------------------------------------
// Display

void draw_ui_user(void) {
    bool            hue_redraw = false;
    static uint16_t last_hue   = 0xFFFF;
    uint8_t         curr_hue   = rgblight_get_hue();
    if (last_hue != curr_hue) {
        last_hue   = curr_hue;
        hue_redraw = true;
    }

    bool            layer_state_redraw = false;
    static uint32_t last_layer_state   = 0;
    if (last_layer_state != layer_state) {
        last_layer_state   = layer_state;
        layer_state_redraw = true;
    }

    bool                     power_state_redraw = false;
    static usbpd_allowance_t last_current_state = (usbpd_allowance_t)(~0);
    if (last_current_state != kb_state.current_setting) {
        last_current_state = kb_state.current_setting;
        power_state_redraw = true;
    }

    bool            scan_redraw      = false;
    static uint32_t last_scan_update = 0;
    if (timer_elapsed32(last_scan_update) > 125) {
        last_scan_update = timer_read32();
        scan_redraw      = true;
    }

    bool            wpm_redraw      = false;
    static uint32_t last_wpm_update = 0;
    if (timer_elapsed32(last_wpm_update) > 125) {
        last_wpm_update = timer_read32();
        wpm_redraw      = true;
    }

#if defined(RGB_MATRIX_ENABLE)
    bool            rgb_effect_redraw = false;
    static uint16_t last_effect       = 0xFFFF;
    uint8_t         curr_effect       = rgb_matrix_config.mode;
    if (last_effect != curr_effect) {
        last_effect       = curr_effect;
        rgb_effect_redraw = true;
    }
#endif  // defined(RGB_MATRIX_ENABLE)

    // Show the Djinn logo and two vertical bars on both sides
    if (hue_redraw) {
        qp_drawimage_recolor(lcd, 120 - gfx_djinn->width / 2, 32, gfx_djinn, curr_hue, 255, 255);
        qp_rect(lcd, 0, 0, 8, 319, curr_hue, 255, 255, true);
        qp_rect(lcd, 231, 0, 239, 319, curr_hue, 255, 255, true);
    }

    int ypos = 4;

    // Show layer info on the left side
    if (is_keyboard_left()) {
        char buf[32] = {0};
        int  xpos    = 16;

#if defined(RGB_MATRIX_ENABLE)
        if (hue_redraw || rgb_effect_redraw) {
            static int max_rgb_xpos = 0;
            xpos                    = 16;
            snprintf_(buf, sizeof(buf), "rgb: %s", rgb_matrix_name(curr_effect));

            for (int i = 5; i < sizeof(buf); ++i) {
                if (buf[i] == 0)
                    break;
                else if (buf[i] == '_')
                    buf[i] = ' ';
                else if (buf[i - 1] == ' ')
                    buf[i] = toupper(buf[i]);
                else if (buf[i - 1] != ' ')
                    buf[i] = tolower(buf[i]);
            }

            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_rgb_xpos < xpos) {
                max_rgb_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_rgb_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);
        }

        ypos += font_thintel15->glyph_height + 4;
#endif  // defined(RGB_MATRIX_ENABLE)

        if (hue_redraw || layer_state_redraw) {
            const char *layer_name = "unknown";
            switch (get_highest_layer(layer_state)) {
                case _QWERTY:
                    layer_name = "qwerty";
                    break;
                case _LOWER:
                    layer_name = "lower";
                    break;
                case _RAISE:
                    layer_name = "raise";
                    break;
                case _ADJUST:
                    layer_name = "adjust";
                    break;
            }

            static int max_layer_xpos = 0;
            xpos                      = 16;
            snprintf_(buf, sizeof(buf), "layer: %s", layer_name);
            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_layer_xpos < xpos) {
                max_layer_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_layer_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);
        }

        ypos += font_thintel15->glyph_height + 4;

        if (hue_redraw || power_state_redraw) {
            static int max_power_xpos = 0;
            xpos                      = 16;
            snprintf_(buf, sizeof(buf), "power: %s", usbpd_str(kb_state.current_setting));
            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_power_xpos < xpos) {
                max_power_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_power_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);
        }

        ypos += font_thintel15->glyph_height + 4;

        if (hue_redraw || scan_redraw) {
            static int max_scans_xpos = 0;
            xpos                      = 16;
            snprintf_(buf, sizeof(buf), "scans: %d", (int)user_state.scan_rate);
            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_scans_xpos < xpos) {
                max_scans_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_scans_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);
        }

        ypos += font_thintel15->glyph_height + 4;

        if (hue_redraw || wpm_redraw) {
            static int max_wpm_xpos = 0;
            xpos                    = 16;
            snprintf_(buf, sizeof(buf), "wpm: %d", (int)get_current_wpm());
            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_wpm_xpos < xpos) {
                max_wpm_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_wpm_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);
        }

        ypos += font_thintel15->glyph_height + 4;
    }

    // Show LED lock indicators on the right side
    if (!is_keyboard_left()) {
        static led_t last_led_state = {0};
        if (hue_redraw || last_led_state.raw != host_keyboard_led_state().raw) {
            last_led_state.raw = host_keyboard_led_state().raw;
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 3), 0, last_led_state.caps_lock ? gfx_lock_caps_ON : gfx_lock_caps_OFF, curr_hue, 255, last_led_state.caps_lock ? 255 : 32);
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 2), 0, last_led_state.num_lock ? gfx_lock_num_ON : gfx_lock_num_OFF, curr_hue, 255, last_led_state.num_lock ? 255 : 32);
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 1), 0, last_led_state.scroll_lock ? gfx_lock_scrl_ON : gfx_lock_scrl_OFF, curr_hue, 255, last_led_state.scroll_lock ? 255 : 32);
        }
    }
}
