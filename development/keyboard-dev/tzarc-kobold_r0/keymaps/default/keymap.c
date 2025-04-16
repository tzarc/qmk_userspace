// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H

#ifndef COMMUNITY_MODULE_FILESYSTEM_ENABLE
#    define FS_DUMP KC_NO
#endif

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_4x4(
        KC_A, KC_B, KC_C, KC_D,
        KC_E, KC_F, KC_G, KC_H,
        KC_I, KC_SFDP, TURBO, FS_DUMP,
        KC_M, DB_TOGG, RM_NEXT, QK_BOOT
    ),
};
// clang-format on

void keyboard_post_init_user() {
    debug_enable = true;
    debug_matrix = true;
}

extern painter_device_t oled;

bool display_task_user(void) {
    static bool filled = false;

    filled = !filled;
    qp_rect(oled, 0, 0, 16, 15, 0, 0, 0, true);
    qp_circle(oled, 8, 8, 8, 0, 0, 255, filled);
    return false;
}
