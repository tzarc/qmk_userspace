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
#include <string.h>
#include <backlight.h>
#include <qp.h>

#include "tzarc.h"
#include "serial_usart_statesync.h"

#include "graphics/djinn.c"
#include "graphics/lock-caps.c"
#include "graphics/lock-scrl.c"
#include "graphics/lock-num.c"
#include "graphics/lock-caps-OFF.c"
#include "graphics/lock-scrl-OFF.c"
#include "graphics/lock-num-OFF.c"
#include "graphics/noto.c"

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
                       KC_LEFT, _______, KC_RIGHT,                                            KC_LEFT, _______, KC_RIGHT,
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
    uint8_t temp_mod = get_mods();
    uint8_t temp_osm = get_oneshot_mods();
    bool    is_ctrl  = (temp_mod | temp_osm) & MOD_MASK_CTRL;
    bool    is_shift = (temp_mod | temp_osm) & MOD_MASK_SHIFT;

    if (is_shift) {
        if (index == 0) { /* First encoder */
            if (clockwise) {
                tap_code16(KC_MS_WH_DOWN);
            } else {
                tap_code16(KC_MS_WH_UP);
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
                rgblight_increase_hue();
            } else {
                rgblight_decrease_hue();
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

//----------------------------------------------------------
// Runtime data sync -- user/keymap

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
static user_runtime_config user_state;

void *get_split_sync_state_user(size_t *state_size) {
    *state_size = sizeof(user_runtime_config);
    return &user_state;
}

bool split_sync_update_task_user(void) {
    if (is_keyboard_master()) {
        // Sync the LED state
        user_state.values.led_state = host_keyboard_led_state();
    }

    // Force an update if the state changed
    static user_runtime_config last_state;
    if (memcmp(&last_state, &user_state, sizeof(user_runtime_config)) != 0) {
        memcpy(&last_state, &user_state, sizeof(user_runtime_config));
        return true;
    }

    return false;
}

void split_sync_action_task_user(void) {
    if (kb_state.values.lcd_power) {
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
            for (int i = 0; i < 8; ++i) qp_circle(lcd, 20, (i * 40) + 20, 10, curr_hue, 255, 255, (i % 2) == 0);
            //            for(int i = 0; i < 8; ++i)
            //                qp_ellipse(lcd, 20, (i * 40) + 20, 20, 12, curr_hue, 255, 255, (i % 2) == 0);
            qp_rect(lcd, 232, 0, 239, 319, curr_hue, 255, 255, true);
            qp_line(lcd, 8, 0, 32, 319, curr_hue, 255, 255);
            qp_line(lcd, 32, 0, 8, 319, curr_hue, 255, 255);
        }

        static led_t last_led_state = {0};
        if (redraw_required || last_led_state.raw != user_state.values.led_state.raw) {
            last_led_state.raw = user_state.values.led_state.raw;
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 3), 0, last_led_state.caps_lock ? gfx_lock_caps : gfx_lock_caps_OFF, curr_hue, 255, last_led_state.caps_lock ? 255 : 32);
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 2), 0, last_led_state.num_lock ? gfx_lock_num : gfx_lock_num_OFF, curr_hue, 255, last_led_state.num_lock ? 255 : 32);
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 1), 0, last_led_state.scroll_lock ? gfx_lock_scrl : gfx_lock_scrl_OFF, curr_hue, 255, last_led_state.scroll_lock ? 255 : 32);
            qp_drawtext(lcd, 0, 0, font_noto, "So this is a test of font rendering");
            qp_drawtext_recolor(lcd, 0, font_noto->glyph_height, font_noto, "with Quantum Painter...", 0, 255, 255, 0, 255, 0);
            qp_drawtext_recolor(lcd, 0, 2*font_noto->glyph_height, font_noto, "Perhaps a different background?", 43, 255, 255, 169, 255, 255);
            qp_drawtext(lcd, 0, 3*font_noto->glyph_height, font_noto, "Unicode: ĄȽɂɻɣɈʣ");
        }
    }
}

void housekeeping_task_keymap(void) {
    // Ensure state is sync'ed from master to slave, if required
    split_sync_user(false);

#ifdef LUA_ENABLE
    void test_lua(void);
    test_lua();
#endif  // LUA_ENABLE
}

#ifdef LUA_ENABLE
#    include <quantum.h>
#    include <lua.h>
#    include <lualib.h>
#    include <lauxlib.h>

lua_State *L                 = 0;
bool       lua_test_executed = false;

static int dprint_wrapper(lua_State *L) {
    const char *arg = luaL_checkstring(L, 1);  // first arg is what we want to print
    dprintf("%s\n", arg);
    return 0;
}

void test_lua(void) {
    if (!lua_test_executed && timer_read32() > 15000) {
        lua_test_executed = true;

        L = luaL_newstate();
        luaL_openlibs(L);

        lua_newtable(L);                                              // new table
        lua_pushnumber(L, 1);                                         // table index
        lua_pushstring(L, "This is a test from executing lua code");  // value
        lua_rawset(L, -3);                                            // set tbl[1]='This is a test from executing lua code'

        // Set the "blah" global table to the newly-created table
        lua_setglobal(L, "blah");

        lua_pushcfunction(L, &dprint_wrapper);
        lua_setglobal(L, "dprint");

        // now we can use blah[1] == 'This is a test from executing lua code'

        const char *code = "dprint(blah[1])";  // should debug print "This is a test from executing lua code" in QMK Toolbox
        if (luaL_loadstring(L, code) == LUA_OK) {
            if (lua_pcall(L, 0, 1, 0) == LUA_OK) {
                lua_pop(L, lua_gettop(L));
            } else {
                dprint("Failed lua_pcall\n");
            }
        } else {
            dprint("Failed luaL_loadstring\n");
        }

        lua_close(L);
    }
}
#endif  // LUA_ENABLE