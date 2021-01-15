/* Copyright 2018-2020 Nick Brassel (@tzarc)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

    rgblight_enable();
}

void matrix_io_delay(void) { __asm__ volatile("nop\nnop\nnop\n"); }
