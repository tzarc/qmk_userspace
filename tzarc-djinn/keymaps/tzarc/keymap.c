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
#include <backlight.h>
#include <qp.h>
#include <printf.h>
#include <transactions.h>

#include "tzarc.h"
#include "qp_rgb565_surface.h"

#include "graphics/src/djinn.c"
#include "graphics/src/lock-caps-ON.c"
#include "graphics/src/lock-scrl-ON.c"
#include "graphics/src/lock-num-ON.c"
#include "graphics/src/lock-caps-OFF.c"
#include "graphics/src/lock-scrl-OFF.c"
#include "graphics/src/lock-num-OFF.c"
#include "graphics/src/thintel15.c"

#define MEDIA_KEY_DELAY 2

painter_device_t surf;

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT_all_wrapper(
        ____________TZARC_7x4_BASE_R1_L__________,                                            ____________TZARC_7x4_BASE_R1_R__________,
        ____________TZARC_7x4_BASE_R2_L__________,                                            ____________TZARC_7x4_BASE_R2_R__________,
        ____________TZARC_7x4_BASE_R3_L__________,                                            ____________TZARC_7x4_BASE_R3_R__________,
        ____________TZARC_7x4_BASE_R4_L__________,                                            ____________TZARC_7x4_BASE_R4_R__________,
                     KC_LGUI, KC_LOWER,  KC_SPC,  KC_NO,                                KC_NO,   KC_SPC,  KC_RAISE,  KC_LALT,
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

bool encoder_update_keymap(uint8_t index, bool clockwise) {
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
    return false;
}

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

static uint32_t counter = 0;

void slave_counter_sync_callback(uint8_t initiator2target_buffer_size, const void *initiator2target_buffer, uint8_t target2initiator_buffer_size, void *target2initiator_buffer) {
    uint32_t *send = (uint32_t *)target2initiator_buffer;
    *send          = ++counter;
}

void keyboard_post_init_keymap(void) {
    // Initialise the framebuffer
    surf = qp_rgb565_surface_make_device(8, 320);
    qp_init(surf, QP_ROTATION_0);
    for (int i = 0; i < 320; ++i) {
        qp_line(surf, 0, i, 7, i, i % 256, 255, 255);
    }

    qp_viewport(lcd, 240 - 8 - 8 - 8, 0, 240 - 8 - 8 - 1, 319);
    qp_pixdata(lcd, qp_rgb565_surface_get_buffer_ptr(surf), qp_rgb565_surface_get_pixel_count(surf));

    // Register keyboard state sync split transaction
    transaction_register_rpc(RPC_ID_SYNC_STATE_USER, rpc_user_sync_callback);
    transaction_register_rpc(RPC_ID_GET_COUNTER, slave_counter_sync_callback);

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
            if (transaction_rpc_send(RPC_ID_SYNC_STATE_USER, sizeof(user_runtime_config), &user_state)) {
                last_sync = timer_read32();
            } else {
                dprint("Failed to perform rpc call\n");
            }

            uint32_t counter;
            if (transaction_rpc_recv(RPC_ID_GET_COUNTER, sizeof(counter), &counter)) {
                dprintf("Slave counter: %d\n", (int)counter);
            } else {
                dprint("Failed to perform rpc call\n");
            }
        }
    }
}

void housekeeping_task_keymap(void) {
    // Update kb_state so we can send to slave
    user_state_update();

    // Data sync from master to slave
    user_state_sync();

#ifdef LUA_ENABLE
    void test_lua(void);
    test_lua();
#endif  // LUA_ENABLE
}

//----------------------------------------------------------
// Display

void draw_ui_user(void) {
    bool            redraw_required = false;
    static uint16_t last_hue        = 0xFFFF;
    uint8_t         curr_hue        = rgblight_get_hue();
    if (last_hue != curr_hue) {
        last_hue        = curr_hue;
        redraw_required = true;
    }

    // Show the Djinn logo and two vertical bars on both sides
    if (redraw_required) {
        qp_drawimage_recolor(lcd, 120 - gfx_djinn->width / 2, 32, gfx_djinn, curr_hue, 255, 255);
        qp_rect(lcd, 0, 0, 8, 319, curr_hue, 255, 255, true);
        qp_rect(lcd, 231, 0, 239, 319, curr_hue, 255, 255, true);
    }

    // Show layer info on the left side
    if (is_keyboard_left()) {
        static uint32_t last_layer_state = 0;
        if (redraw_required || last_layer_state != layer_state) {
            last_layer_state = layer_state;

            const char *layer_name = "unknown";
            switch (get_highest_layer(layer_state)) {
                case LAYER_BASE:
                    layer_name = "qwerty";
                    break;
                case LAYER_LOWER:
                    layer_name = "lower";
                    break;
                case LAYER_RAISE:
                    layer_name = "raise";
                    break;
                case LAYER_ADJUST:
                    layer_name = "adjust";
                    break;
            }

            static int max_xpos = 0;
            int        xpos     = 16;
            int        ypos     = 4;
            char       buf[32]  = {0};
            snprintf_(buf, sizeof(buf), "layer: %s", layer_name);
            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_xpos < xpos) {
                max_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);
        }

        static uint32_t last_scan_update = 0;
        if (redraw_required || timer_elapsed32(last_scan_update) > 125) {
            last_scan_update = timer_read32();

            static int max_xpos = 0;
            int        xpos     = 16;
            int        ypos     = 4 + font_thintel15->glyph_height + 4;
            char       buf[32]  = {0};
            snprintf_(buf, sizeof(buf), "scans: %d", (int)user_state.scan_rate);
            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_xpos < xpos) {
                max_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);

            xpos = 16;
            ypos += 4 + font_thintel15->glyph_height;
            snprintf_(buf, sizeof(buf), "power: %s", usbpd_str(kb_state.current_setting));
            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_xpos < xpos) {
                max_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);

            xpos = 16;
            ypos += 4 + font_thintel15->glyph_height;
            snprintf_(buf, sizeof(buf), "wpm: %d", (int)get_current_wpm());
            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_xpos < xpos) {
                max_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);
        }
    }

    // Show LED lock indicators on the right side
    if (!is_keyboard_left()) {
        static led_t last_led_state = {0};
        if (redraw_required || last_led_state.raw != host_keyboard_led_state().raw) {
            last_led_state.raw = host_keyboard_led_state().raw;
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 3), 0, last_led_state.caps_lock ? gfx_lock_caps_ON : gfx_lock_caps_OFF, curr_hue, 255, last_led_state.caps_lock ? 255 : 32);
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 2), 0, last_led_state.num_lock ? gfx_lock_num_ON : gfx_lock_num_OFF, curr_hue, 255, last_led_state.num_lock ? 255 : 32);
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 1), 0, last_led_state.scroll_lock ? gfx_lock_scrl_ON : gfx_lock_scrl_OFF, curr_hue, 255, last_led_state.scroll_lock ? 255 : 32);
        }
    }

#if 0
    // Test code
    if (redraw_required) {
        qp_line(lcd, 8, 0, 32, 319, curr_hue, 255, 255);
        qp_line(lcd, 32, 0, 8, 319, curr_hue, 255, 255);
        for (int i = 0; i < 8; ++i) qp_circle(lcd, 20, (i * 40) + 20, 10, curr_hue, 255, 255, (i % 2) == 0);
        qp_drawtext(lcd, 0, 0, font_noto16, "So this is a test of font rendering");
        qp_drawtext_recolor(lcd, 0, font_noto16->glyph_height, font_noto16, "with Quantum Painter...", 0, 255, 255, 0, 255, 0);
        qp_drawtext_recolor(lcd, 0, 2 * font_noto16->glyph_height, font_noto16, "Perhaps a different background?", 43, 255, 255, 169, 255, 255);
        qp_drawtext(lcd, 0, 3 * font_noto16->glyph_height, font_noto28, "Unicode: ĄȽɂɻɣɈʣ");
        qp_drawtext(lcd, 0, 3 * font_noto16->glyph_height + font_noto28->glyph_height, font_redalert13, "And here we are, with another font!");
    }
#endif
}
//----------------------------------------------------------
// Lua

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