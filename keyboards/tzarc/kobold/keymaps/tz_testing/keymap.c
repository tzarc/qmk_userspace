// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H

#ifndef COMMUNITY_MODULE_FILESYSTEM_ENABLE
#    define FS_DUMP KC_NO
#endif

#ifndef COMMUNITY_MODULE_SFDP_FLASH_ENABLE
#    define KC_SFDP KC_NO
#endif

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_4x4(
        KC_A, KC_B, KC_C, KC_D,
        KC_E, KC_F, KC_G, KC_H,
        KC_I, KC_SFDP, TURBO, FS_DUMP,
        EE_CLR, DB_TOGG, RM_NEXT, QK_BOOT
    ),
};
// clang-format on

/*
void eeconfig_init_user(void) {
    debug_enable = true;
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv(0, 255, 20);
}

void keyboard_post_init_user(void) {
    debug_enable = true;
    debug_matrix = true;
}

static uint32_t should_reset_at = ~0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch(keycode) {
        case EE_CLR:
        {
            if (record->event.pressed) {
                eeconfig_init();
                should_reset_at = timer_read32() + 2000;
            }
            return false;
        }
    }
    return true;
}

void housekeeping_task_user(void) {
    if(should_reset_at < timer_read32()) {
        mcu_reset();
    }
}
*/
