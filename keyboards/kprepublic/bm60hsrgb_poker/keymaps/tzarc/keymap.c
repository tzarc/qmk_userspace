// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

// Reroute to the layout instead.
#include "layouts/60_ansi/tzarc/keymap.c"

void keyboard_post_init_keymap(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_MULTISPLASH);
    rgb_matrix_sethsv(212, 255, 255);
}
