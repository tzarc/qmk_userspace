// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define MATRIX_ROWS 1
#define MATRIX_COLS 1

#define VENDOR_ID 0xFEED
#define PRODUCT_ID 0xCAFE
#define DEVICE_VER 0x0001
#define MANUFACTURER "Tzarc"
#define PRODUCT "LVGL Tester"

// Matrix config
#define MATRIX_ROW_PINS \
    { C14 }
#define MATRIX_COL_PINS \
    { C15 }
#define DIODE_DIRECTION COL2ROW

// SPI config
#define SPI_DRIVER SPID1
#define SPI_SCK_PIN A5
#define SPI_SCK_PAL_MODE 5
#define SPI_MOSI_PIN A7
#define SPI_MOSI_PAL_MODE 5
#define SPI_MISO_PIN A6
#define SPI_MISO_PAL_MODE 5

// 240x320 ST7789
#define DISPLAY_DC_PIN B5
#define DISPLAY_CS_PIN B6
#define DISPLAY_RST_PIN B4

#define QUANTUM_PAINTER_DEBUG
