// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// 1000Hz poll rate
#define USB_POLLING_INTERVAL_MS 1

// We don't ever have more than 8 layers.
#define LAYER_STATE_8BIT

// Unicode modes available for use
#define UNICODE_SELECTED_MODES UC_MAC, UC_LNX, UC_WINC

// Handle per-key tapping timing
#ifdef TAPPING_TERM
#    undef TAPPING_TERM
#endif // TAPPING_TERM
#define TAPPING_TERM 120

#define RETRO_TAPPING
#define TAPPING_TERM_PER_KEY
#define PERMISSIVE_HOLD
#define PERMISSIVE_HOLD_PER_KEY
#define IGNORE_MOD_TAP_INTERRUPT
#define IGNORE_MOD_TAP_INTERRUPT_PER_KEY
#define TAPPING_FORCE_HOLD
#define TAPPING_FORCE_HOLD_PER_KEY

// Pre-define the amount of space to use for userspace EEPROM
#define EECONFIG_USER_DATA_SIZE 32
#define EECONFIG_USER_DATA_VERSION ((uint32_t)__TIME__[0] + (uint32_t)__TIME__[1] + (uint32_t)__TIME__[3] + (uint32_t)__TIME__[4] + (uint32_t)__TIME__[6] + (uint32_t)__TIME__[7]) // HH:MM::SS

#if !defined(VIA_ENABLE)
// Modify the RAW usage page and id
#    define RAW_USAGE_PAGE 0xFF9C
#    define RAW_USAGE_ID 0x02
#endif

// Ensure transient EEPROM has enough space
#ifdef EEPROM_TRANSIENT
#    define EEPROM_SIZE 96
#endif // EEPROM_TRANSIENT

// Slightly larger debounce
#ifdef DEBOUNCE
#    undef DEBOUNCE
#endif
#define DEBOUNCE 12
