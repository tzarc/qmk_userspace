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

#include "config_common.h"

//#define EARLY_INIT_PERFORM_BOOTLOADER_JUMP TRUE

#define MATRIX_ROW_PINS \
    { B5 }
#define MATRIX_COL_PINS \
    { B4 }
#define UNUSED_PINS

#define SOFT_SERIAL_PIN A9
#define SERIAL_USART_TX_PAL_MODE 1
#define SERIAL_USART_SPEED 460800  // 9600 // 38400 // 115200 // 230400 // 460800 // 921600 // 1843200 // 2000000

#define SPLIT_USB_DETECT
#define SPLIT_HAND_PIN A2

#define DEBUG_MATRIX_SCAN_RATE
