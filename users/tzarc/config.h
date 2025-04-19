// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// 1000Hz poll rate
#define USB_POLLING_INTERVAL_MS 1

#define FILESYSTEM_DEBUG

// We don't ever have more than 8 layers.
#define LAYER_STATE_8BIT

// Unicode modes available for use
#define UNICODE_SELECTED_MODES UNICODE_MODE_MACOS, UNICODE_MODE_LINUX, UNICODE_MODE_WINCOMPOSE

// Pre-define the amount of space to use for userspace EEPROM
#define EECONFIG_USER_DATA_SIZE 32

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

#ifndef __AVR__
#    include "enable_all_rgb_effects.h"
#endif // !__AVR__

// #define QUANTUM_PAINTER_DEBUG TRUE
#define QUANTUM_PAINTER_SUPPORTS_256_PALETTE TRUE
#define QUANTUM_PAINTER_SUPPORTS_NATIVE_COLORS TRUE
