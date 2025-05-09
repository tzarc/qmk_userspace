// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "tzarc.h"

// Reroute to the layout instead.
#include "layouts/ortho_4x4/tzarc/keymap.c"

bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TZ_TM0:
            if (record->event.pressed) {
                char buffer[16];
                strcpy(buffer, "Test RAM");
                send_string(buffer);
            }
            return false;
        case TZ_TM1:
            if (record->event.pressed) {
                static const char buffer[] PROGMEM = "Test PROGMEM";
                send_string_P(buffer);
            }
            return false;
    }
    return true;
}
