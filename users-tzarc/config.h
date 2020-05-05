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

// Pre-define the amount of space to use for userspace EEPROM
#define TZARC_EEPROM_ALLOCATION 32

// If we ever decide we're going to use VIA, then make sure we're not going to collide with any of the EEPROM settings for userspace.
#define VIA_EEPROM_MAGIC_ADDR (EECONFIG_SIZE + TZARC_EEPROM_ALLOCATION + 1)

// #define DEBUG_MATRIX_SCAN_RATE
