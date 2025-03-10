// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "config_common.h"

// Matrix
#define MATRIX_SHIFT_REGISTER_COUNT 5
// #define MATRIX_ROWS 6 // actually defined in info.json: 5 shift registers, plus one row for extras (i.e. encoder pushbutton read)
// #define MATRIX_COLS 8 // actually defined in info.json: 8 bits per register

// EEPROM configuration
#define EXTERNAL_EEPROM_BYTE_COUNT 8192
#define EXTERNAL_EEPROM_PAGE_SIZE 64 // it's FRAM, so it doesn't actually matter, this just sets the RAM buffer

// RGB configuration
#define RGB_MATRIX_LED_COUNT 40
#define RGBLED_NUM 40       // TBD: Once convergence with these defines occurs, remove.
#define DRIVER_LED_TOTAL 40 // TBD: Once convergence with these defines occurs, remove.
