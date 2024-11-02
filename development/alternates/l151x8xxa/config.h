// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID 0x00DE
#define PRODUCT_ID 0x0080
#define DEVICE_VER 0x0076 // L for L151 version
#define MANUFACTURER Mode
#define PRODUCT Eighty

/* key matrix size */
#define MATRIX_ROWS 1
#define MATRIX_COLS 1

#define DIODE_DIRECTION COL2ROW

#define MATRIX_COL_PINS {B8}
#define MATRIX_ROW_PINS {A10}

#define STM32_HSECLK 8000000U
