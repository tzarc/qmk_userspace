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

#define MATRIX_ROWS 8
#define MATRIX_COLS 8
#define DIODE_DIRECTION COL2ROW

#define VENDOR_ID 0xFEED
#define PRODUCT_ID 0xCAFE
#define DEVICE_VER 0x0001
#define MANUFACTURER Me
#define PRODUCT Thing

#define SPI_DRIVER SPID1
#define PAL_STM32_OSPEED_HIGHEST PAL_STM32_OSPEED_HIGH

#define DEBUG_MATRIX_SCAN_RATE

//#define SPI_MATRIX_LATCH_PIN A8
//#define SPI_MATRIX_PLOAD_PIN A9
#define SPI_MATRIX_CHIP_SELECT_PIN B12
#define SPI_MATRIX_SCK_PIN B13
#define SPI_MATRIX_MISO_PIN B14
#define SPI_MATRIX_MOSI_PIN B15

#define SPI_SCK_PAL_MODE 0
#define SPI_MOSI_PAL_MODE 0
#define SPI_MISO_PAL_MODE 0
