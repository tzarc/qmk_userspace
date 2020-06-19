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

#include "djinn.h"

void matrix_io_delay(void) { __asm__ volatile("nop\nnop\nnop\n"); }

void keyboard_post_init_kb(void) {
    debug_enable = true;
    debug_matrix = true;

    // Turn on the LCD
    setPinOutput(LCD_POWER_ENABLE_PIN);
    writePinLow(LCD_POWER_ENABLE_PIN);

    // Turn on the RGB
    setPinOutput(RGB_POWER_ENABLE_PIN);
    writePinLow(RGB_POWER_ENABLE_PIN);
}