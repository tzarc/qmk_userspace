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

#pragma once

//----------------------------------------------------------
// Runtime data sync -- keyboard

enum { current_500mA = 0, current_1500mA, current_3000mA };

#pragma pack(push)
#pragma pack(1)

typedef union kb_runtime_config {
    struct {
        unsigned lcd_power : 1;
        unsigned current_setting : 2;
    } values;
    uint8_t raw;
} kb_runtime_config;

#pragma pack(pop)

_Static_assert(sizeof(kb_runtime_config) == 1, "Invalid data transfer size for keyboard runtime data");