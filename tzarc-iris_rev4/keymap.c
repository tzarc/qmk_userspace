// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
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
#include <print.h>
#include <string.h>
#include <transactions.h>

uint32_t counter = 0;

void slave_counter_sync(uint8_t initiator2target_buffer_size, const void* initiator2target_buffer, uint8_t target2initiator_buffer_size, void* target2initiator_buffer) {
    uint32_t* send = (uint32_t*)target2initiator_buffer;
    *send          = timer_read32() / 1000;
}

void keyboard_post_init_keymap(void) {
    // Register keyboard state sync split transaction
    transaction_register_rpc(RPC_ID_SLAVE_COUNTER, slave_counter_sync);
}

void user_state_sync(void) {
    if (is_keyboard_master()) {
        // Send to slave every 500ms
        static uint32_t last_sync = 0;
        if (timer_elapsed32(last_sync) > 500) {
            last_sync = timer_read32();
            transaction_rpc_recv(RPC_ID_SLAVE_COUNTER, sizeof(counter), &counter);
            dprintf("Slave timer: %d\n", (int)counter);
        }
    }
}

void housekeeping_task_keymap(void) {
    // Data sync from master to slave
    user_state_sync();
}
