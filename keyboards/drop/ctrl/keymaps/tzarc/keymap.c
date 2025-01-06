// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

// Apparently rules.mk and config.h aren't picked up unless there's a keymap.c in this directory.

// Total hack to get the same damn layout, why isn't it a LAYOUT_tkl_ansi?
#define LAYOUT_tkl_ansi(...) LAYOUT(__VA_ARGS__)

// Reroute to the layout instead.
#include "layouts/tkl_ansi/tzarc/keymap.c"

void keyboard_post_init_keymap(void) {
    rgb_matrix_mode(RGB_MATRIX_SOLID_MULTISPLASH);
    rgb_matrix_sethsv(104, 255, 255);
}
