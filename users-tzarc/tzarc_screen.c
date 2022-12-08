// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "tzarc.h"

#ifdef KEYBOARD_tzarc_djinn

// Adapt the existing implementation
#    define draw_ui_user theme_draw_ui_user
#    include "theme_djinn_default.c"
#    undef draw_ui_user

// Adapt the keyboard drawing target
#    define display_panel lcd
#    define draw_screen_base(force)    \
        do {                           \
            theme_draw_ui_user(force); \
        } while (0)

// Pull in the userspace screen drawing functions
#    include "tzarc_screen_qp_240x320.c"

// Actual draw "hook"
void draw_ui_user(bool force_redraw) {
    draw_screen(force_redraw);
}

#endif
