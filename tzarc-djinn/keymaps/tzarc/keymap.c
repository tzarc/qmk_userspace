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

#include "tzarc.h"
//#include "qp_rgb565_surface.h"

#define MEDIA_KEY_DELAY 5

painter_device_t surf;

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT_all_wrapper(
        ____________TZARC_7x4_BASE_R1_L__________,                                            ____________TZARC_7x4_BASE_R1_R__________,
        ____________TZARC_7x4_BASE_R2_L__________,                                            ____________TZARC_7x4_BASE_R2_R__________,
        ____________TZARC_7x4_BASE_R3_L__________,                                            ____________TZARC_7x4_BASE_R3_R__________,
        ____________TZARC_7x4_BASE_R4_L__________,                                            ____________TZARC_7x4_BASE_R4_R__________,
                     KC_LGUI, KC_LOWER,  KC_SPC,  SH_TG,                                SH_TG,  KC_SPC,  KC_RAISE,  KC_LALT,
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
                     _______, _______, _______, _______,                                _______, _______, _______, _______,
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
                     _______, _______, _______, _______,                                _______, _______, _______, _______,
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
                     _______, _______, _______, _______,                                _______, _______, _______, _______,
                                                           _______,           _______,
                                _______,                                                             _______,
                       _______, _______, _______,                                           _______, _______, _______,
                                _______,                                                             _______
    )
};
// clang-format on

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [LAYER_BASE]   = {ENCODER_CCW_CW(KC_MS_WH_UP, KC_MS_WH_DOWN), ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [LAYER_LOWER]  = {ENCODER_CCW_CW(RGB_HUD, RGB_HUI), ENCODER_CCW_CW(RGB_SAD, RGB_SAI)},
    [LAYER_RAISE]  = {ENCODER_CCW_CW(RGB_VAD, RGB_VAI), ENCODER_CCW_CW(RGB_SPD, RGB_SPI)},
    [LAYER_ADJUST] = {ENCODER_CCW_CW(RGB_RMOD, RGB_MOD), ENCODER_CCW_CW(KC_LEFT, KC_RIGHT)},
};
#else
bool encoder_update_keymap(uint8_t index, bool clockwise) {
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

// clang-format off
#ifdef SWAP_HANDS_ENABLE
const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
   { { 6,  6 }, { 5,  6 }, { 4,  6 }, { 3,  6 }, { 2,  6 }, { 1,  6 }, { 0,  6 } },
   { { 6,  7 }, { 5,  7 }, { 4,  7 }, { 3,  7 }, { 2,  7 }, { 1,  7 }, { 0,  7 } },
   { { 6,  8 }, { 5,  8 }, { 4,  8 }, { 3,  8 }, { 2,  8 }, { 1,  8 }, { 0,  8 } },
   { { 6,  9 }, { 5,  9 }, { 4,  9 }, { 3,  9 }, { 2,  9 }, { 1,  9 }, { 0,  9 } },
   { { 0,  0 }, { 0,  0 }, { 0,  0 }, { 6, 10 }, { 5, 10 }, { 4, 10 }, { 3, 10 } },
   { { 0,  0 }, { 6, 11 }, { 5, 11 }, { 4, 11 }, { 3, 11 }, { 2, 11 }, { 1, 11 } },

   { { 6,  0 }, { 5,  0 }, { 4,  0 }, { 3,  0 }, { 2,  0 }, { 1,  0 }, { 0,  0 } },
   { { 6,  1 }, { 5,  1 }, { 4,  1 }, { 3,  1 }, { 2,  1 }, { 1,  1 }, { 0,  1 } },
   { { 6,  2 }, { 5,  2 }, { 4,  2 }, { 3,  2 }, { 2,  2 }, { 1,  2 }, { 0,  2 } },
   { { 6,  3 }, { 5,  3 }, { 4,  3 }, { 3,  3 }, { 2,  3 }, { 1,  3 }, { 0,  3 } },
   { { 0,  0 }, { 0,  0 }, { 0,  0 }, { 6,  4 }, { 5,  4 }, { 4,  4 }, { 3,  4 } },
   { { 0,  0 }, { 6,  5 }, { 5,  5 }, { 4,  5 }, { 3,  5 }, { 2,  5 }, { 1,  5 } },
};
#    ifdef ENCODER_MAP_ENABLE
const uint8_t PROGMEM encoder_hand_swap_config[NUM_ENCODERS] = { 1, 0 };
#    endif
#endif
// clang-format on

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
    /*
    // Initialise the framebuffer
    surf = qp_rgb565_surface_make_device(8, 320);
    qp_init(surf, QP_ROTATION_0);
    for (int i = 0; i < 320; ++i) {
        qp_line(surf, 0, i, 7, i, i % 256, 255, 255);
    }

    qp_viewport(lcd, 240 - 8 - 8 - 8, 0, 240 - 8 - 8 - 1, 319);
    qp_pixdata(lcd, qp_rgb565_surface_get_buffer_ptr(surf), qp_rgb565_surface_get_pixel_count(surf));
    */

    // Register keyboard state sync split transaction
    transaction_register_rpc(RPC_ID_SYNC_STATE_USER, rpc_user_sync_callback);
    transaction_register_rpc(RPC_ID_GET_COUNTER, slave_counter_sync_callback);

    // Reset the initial shared data value between master and slave
    memset(&user_state, 0, sizeof(user_state));

    void keyboard_post_init_display(void);
    keyboard_post_init_display();
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
            if (transaction_rpc_send(RPC_ID_SYNC_STATE_USER, sizeof(user_runtime_config), &user_state)) {
                last_sync = timer_read32();
            } else {
                dprint("Failed to perform rpc call\n");
            }

            uint32_t counter;
            if (transaction_rpc_recv(RPC_ID_GET_COUNTER, sizeof(counter), &counter)) {
                // dprintf("Slave counter: %d\n", (int)counter);
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
#include "theme_djinn.inl.c"
//#include "theme_hf.inl.c"

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
    (void)arg;
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