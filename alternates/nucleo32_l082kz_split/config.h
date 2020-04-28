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

#define MATRIX_COL_PINS \
    { B4 }
#define MATRIX_ROW_PINS \
    { B5 }
#define UNUSED_PINS

#define SPLIT_USB_DETECT
#define SPLIT_HAND_PIN A0

#define SOFT_SERIAL_PIN A9
#define SERIAL_USART_TX_PAL_MODE 4

#define RGB_DI_PIN A2
#define WS2812_PWM_DRIVER PWMD2
#define WS2812_PWM_CHANNEL 3
#define WS2812_PWM_PAL_MODE 2
#define WS2812_DMA_STREAM STM32_DMA1_STREAM2
#define WS2812_DMA_CHANNEL 8
#define RGBLIGHT_LIMIT_VAL 100

#define SPLIT_USB_TIMEOUT 3000
#define SPLIT_USB_TIMEOUT_POLL 100
#define SERIAL_USART_SPEED 500000  // 9600 // 38400 // 115200 // 230400 // 460800 // 921600 // 1843200 // 2000000

#define DEBUG_MATRIX_SCAN_RATE

#define MATRIX_IO_DELAY 10
