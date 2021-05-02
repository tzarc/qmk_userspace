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

// Apparently rules.mk and config.h aren't picked up unless there's a keymap.c in this directory.

// Total hack to get the same damn layout, why isn't it a LAYOUT_tkl_ansi?
#define LAYOUT_tkl_ansi(...) LAYOUT(__VA_ARGS__)

// Reroute to the layout instead.
#include "layouts/community/tkl_ansi/tzarc/keymap.c"

void keyboard_post_init_keymap(void) {
    rgb_matrix_mode(RGB_MATRIX_PIXEL_FRACTAL);
    rgb_matrix_sethsv(104, 255, 255);
}