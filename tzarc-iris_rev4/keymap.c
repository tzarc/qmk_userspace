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
#ifdef RGBLIGHT_ENABLE
    rgblight_mode(RGBLIGHT_MODE_STATIC_LIGHT);
    rgblight_sethsv(58, 255, 80);
#endif

#ifdef BACKLIGHT_ENABLE
    backlight_enable();
    backlight_level(BACKLIGHT_LEVELS);
#endif
}

//----------------------------------------------------------
// Sync
#if 0
#include <string.h>
#include <transaction_id_define.h>

typedef struct user_runtime_config {
    uint32_t layer_state;
    led_t    led_state;
} user_runtime_config;

typedef struct user_slave_data {
    uint32_t counter;
} user_slave_data;

_Static_assert(sizeof(user_runtime_config) == 5, "Invalid data transfer size for user sync data");
_Static_assert(sizeof(user_slave_data) == 4, "Invalid data transfer size for slave sync data");

user_runtime_config user_state;
user_slave_data     user_slave;

void keyboard_post_init_keymap(void) {
    // Register keyboard state sync split transaction
    split_register_m2s_shmem(USER_STATE_SYNC, sizeof(user_state), &user_state);
    split_register_s2m_shmem(USER_SLAVE_SYNC, sizeof(user_slave), &user_slave);

    // Reset the initial shared data value between master and slave
    memset(&user_state, 0, sizeof(user_state));
    memset(&user_slave, 0, sizeof(user_slave));
}

void user_state_update(void) {
    if (is_keyboard_master()) {
        // Keep the LED state in sync
        user_state.led_state = host_keyboard_led_state();

        // Keep the layer state in sync
        user_state.layer_state = layer_state;
    } else {
        static uint32_t last_increment = 0;
        if (timer_elapsed32(last_increment) > 500) {
            last_increment = timer_read32();
            user_slave.counter++;
        }
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

        // Send to slave every 500ms regardless of state change
        if (timer_elapsed32(last_sync) > 500) {
            needs_sync = true;
        }

        // Perform the sync if requested
        if (needs_sync) {
            last_sync = timer_read32();
            if (!split_sync_shmem(USER_STATE_SYNC)) {
                dprint("Failed to perform sync data transaction\n");
            }
            if (!split_sync_shmem(USER_SLAVE_SYNC)) {
                dprint("Failed to perform slave data transaction\n");
            }

            dprintf("Slave counter: %d\n", (int)user_slave.counter);
        }
    }
}

void housekeeping_task_keymap(void) {
    // Update kb_state so we can send to slave
    user_state_update();

    // Data sync from master to slave
    user_state_sync();
}
#endif // 0