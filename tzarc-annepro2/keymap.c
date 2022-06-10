// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

// Reroute to the layout instead.
#include "layouts/community/60_ansi/tzarc/keymap.c"

void keyboard_post_init_keymap(void) {
    rgb_matrix_mode(RGB_MATRIX_SOLID_MULTISPLASH);
    rgb_matrix_sethsv(104, 255, 255);
}
