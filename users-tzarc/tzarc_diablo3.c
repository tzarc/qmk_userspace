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

#include <string.h>
#include <quantum.h>
#include "tzarc.h"

struct diablo3_config_t diablo3_config;

void tzarc_diablo3_init(void) { memset(&diablo3_config, 0, sizeof(diablo3_config)); }

bool process_record_diablo3(uint16_t keycode, keyrecord_t *record) {
    return process_record_keymap(keycode, record);
    ;
}

void matrix_scan_diablo3(void) {}