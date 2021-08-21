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

// 1000Hz poll rate
#define USB_POLLING_INTERVAL_MS 1

// We don't ever have more than 8 layers.
#define LAYER_STATE_8BIT

// Handle per-key tapping timing
#ifdef TAPPING_TERM
#    undef TAPPING_TERM
#endif  // TAPPING_TERM
#define TAPPING_TERM 120

/*
#define RETRO_TAPPING
#define TAPPING_TERM_PER_KEY
#define PERMISSIVE_HOLD
#define PERMISSIVE_HOLD_PER_KEY
#define IGNORE_MOD_TAP_INTERRUPT
#define IGNORE_MOD_TAP_INTERRUPT_PER_KEY
#define TAPPING_FORCE_HOLD
#define TAPPING_FORCE_HOLD_PER_KEY
*/

// Pre-define the amount of space to use for userspace EEPROM
#define TZARC_EEPROM_ALLOCATION 32

#if !defined(VIA_ENABLE)
// Modify the RAW usage page and id
#    define RAW_USAGE_PAGE 0xFF9C
#    define RAW_USAGE_ID 0x02
#endif

// If we ever decide we're going to use VIA, then make sure we're not going to collide with any of the EEPROM settings for userspace.
// Completely incompatible with the change in RAW usage page and ID.
#define VIA_EEPROM_MAGIC_ADDR (EECONFIG_SIZE + TZARC_EEPROM_ALLOCATION + 1)

// Disable old stuff
#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION
