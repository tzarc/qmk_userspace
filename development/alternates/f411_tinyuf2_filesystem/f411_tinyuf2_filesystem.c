// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H

void keyboard_post_init_user() {
    // This should be in a keymap, but eh
    debug_enable = true;
    debug_matrix = true;
}
