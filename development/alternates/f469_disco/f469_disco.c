// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

void matrix_init_pins(void) {
    // This pin is pulled low by default, but the normal direct pin init initialises each as input high.
    gpio_set_pin_input(A0);
}
