// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#define MATRIX_ROWS 8
#define MATRIX_COLS 8
#define DIODE_DIRECTION COL2ROW

#define VENDOR_ID 0xFEED
#define PRODUCT_ID 0xCAFE
#define DEVICE_VER 0x0001
#define MANUFACTURER Me
#define PRODUCT Thing

#define DEBUG_MATRIX_SCAN_RATE

// #define SPI_MATRIX_LATCH_PIN A8
// #define SPI_MATRIX_PLOAD_PIN A9
#define SPI_MATRIX_CHIP_SELECT_PIN B12
#define SPI_MATRIX_SCK_PIN B13
#define SPI_MATRIX_MISO_PIN B14
#define SPI_MATRIX_MOSI_PIN B15

#define SPI_SCK_PAL_MODE 0
#define SPI_MOSI_PAL_MODE 0
#define SPI_MISO_PAL_MODE 0
