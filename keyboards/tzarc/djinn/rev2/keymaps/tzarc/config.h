// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#ifdef EEPROM_CUSTOM
#    define EEPROM_SIZE 4096
#endif

// Uncomment to spit out debugging info whenever an action is executed.
// #define DEBUG_ACTION

// Uncomment the following if your board uses 1.5A and 3.0A hold current fuses.
// #define DJINN_SUPPORTS_3A_FUSE

// #define QUANTUM_PAINTER_DEBUG
// #define DEBUG_EEPROM_OUTPUT
// #define WEAR_LEVELING_DEBUG_OUTPUT
// #define DEBUG_FLASH_SPI_OUTPUT
// #define WEAR_LEVELING_BACKING_SIZE 16384
// #define WEAR_LEVELING_LOGICAL_SIZE 4096

#define QUANTUM_PAINTER_SUPPORTS_256_PALETTE TRUE
#define QUANTUM_PAINTER_PIXDATA_BUFFER_SIZE 8192

#ifdef BACKLIGHT_LIMIT_VAL
#    undef BACKLIGHT_LIMIT_VAL
#endif // BACKLIGHT_LIMIT_VAL
#define BACKLIGHT_LIMIT_VAL 255

// Encoder settings
#define ENCODER_RESOLUTION 2

// LCD blanking period
#define LCD_ACTIVITY_TIMEOUT 30000

// RGB settings
#define RGB_MATRIX_KEYPRESSES
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS

// Allow for an extra sync command over the split
#define SPLIT_TRANSACTION_IDS_USER THEME_DATA_SYNC

// RGB Effects
#include "enable_all_rgb_effects.h"

// Filesystem debugging
#define FILESYSTEM_DEBUG
