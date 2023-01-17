// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Matrix config
#define MATRIX_ROWS 1
#define MATRIX_COLS 1
#define MATRIX_ROW_PINS \
    { C14 }
#define MATRIX_COL_PINS \
    { C15 }

// I2C config
#define I2C_DRIVER I2CD1
#define I2C1_SCL_PIN A9
#define I2C1_SCL_PAL_MODE 4
#define I2C1_SDA_PIN A10
#define I2C1_SDA_PAL_MODE 4
#define I2C1_TIMINGR_PRESC 0U
#define I2C1_TIMINGR_SCLDEL 11U
#define I2C1_TIMINGR_SDADEL 0U
#define I2C1_TIMINGR_SCLH 14U
#define I2C1_TIMINGR_SCLL 42U

// SPI config
#define SPI_DRIVER SPID1
#define SPI_SCK_PIN A5
#define SPI_SCK_PAL_MODE 5
#define SPI_MOSI_PIN A7
#define SPI_MOSI_PAL_MODE 5
#define SPI_MISO_PIN A6
#define SPI_MISO_PAL_MODE 5

// QP config
#define QUANTUM_PAINTER_CONCURRENT_ANIMATIONS 8
#define QUANTUM_PAINTER_LOAD_FONTS_TO_RAM TRUE

// Display common config
#define DISPLAY_DC_PIN B5

// 240x320 ST7789
#define DISPLAY_CS_PIN_2_0_INCH_LCD_ST7789 B6
#define DISPLAY_RST_PIN_2_0_INCH_LCD_ST7789 B4

// Debugging output
#define DEBUG_EEPROM_OUTPUT
#define WEAR_LEVELING_DEBUG_OUTPUT
#define DEBUG_FLASH_SPI_OUTPUT
#define QUANTUM_PAINTER_DEBUG
