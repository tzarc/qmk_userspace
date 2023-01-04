// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <quantum.h>

/*
static uint32_t matrix_scan_user_timeout = 0;
static bool matrix_scan_user_status = false;
static uint32_t matrix_slave_scan_user_timeout = 0;
static bool matrix_slave_scan_user_status = false;

void matrix_scan_user(void) {
    uint32_t now = timer_read32();
    if(timer_expired32(now, matrix_scan_user_timeout)) {
        matrix_scan_user_timeout = now + 1000;
        matrix_scan_user_status = !matrix_scan_user_status;
        setPinOutput(A7);
        writePin(A7, matrix_scan_user_status);
    }

    setPinOutput(B3);
    writePinHigh(B3);
}

void matrix_slave_scan_user(void) {
    uint32_t now = timer_read32();
    if(timer_expired32(now, matrix_slave_scan_user_timeout)) {
        matrix_slave_scan_user_timeout = now + 1000;
        matrix_slave_scan_user_status = !matrix_slave_scan_user_status;
        setPinOutput(A6);
        writePin(A6, matrix_slave_scan_user_status);
    }

    setPinOutput(B3);
    writePinHigh(B3);
}
*/

void keyboard_post_init_user(void) {
    // Customise these values to desired behaviour
    debug_enable = true;
    debug_matrix = true;
    // debug_keyboard=true;
    // debug_mouse=true;
}
