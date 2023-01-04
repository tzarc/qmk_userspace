// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

// Apparently rules.mk and config.h aren't picked up unless there's a keymap.c in this directory.
// Reroute to the layout instead.
#include "layouts/community/tkl_ansi/tzarc/keymap.c"

void keyboard_post_init_keymap(void) {
    debug_enable = true;
    debug_matrix = true;
}
