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

#define EARLY_INIT_PERFORM_BOOTLOADER_JUMP TRUE

#define HAL_USE_SERIAL TRUE
#define SERIAL_BUFFERS_SIZE 1024

#define MATRIX_ROW_PINS \
    { A13 }
#define MATRIX_COL_PINS \
    { A14 }
#define UNUSED_PINS

#define SOFT_SERIAL_PIN A9

#define SERIAL_USART_SPEED 2000000  // 9600 // 38400 // 115200 // 230400 // 460800 // 921600 // 1843200 // 2000000

#define SPLIT_USB_DETECT
#define SPLIT_HAND_PIN A0

#define DEBUG_MATRIX_SCAN_RATE

// #define MATRIX_IO_DELAY 10

#define RGB_DI_PIN B9
#define WS2812_PWM_DRIVER PWMD4
#define WS2812_PWM_CHANNEL 4
#define WS2812_PWM_PAL_MODE 2
#define RGBLIGHT_LIMIT_VAL 100
#define RGBLIGHT_ANIMATIONS

#define BACKLIGHT_PIN B8
#define BACKLIGHT_PWM_DRIVER PWMD4
#define BACKLIGHT_PWM_CHANNEL 3
#define BACKLIGHT_PAL_MODE 2

#define BACKLIGHT_BREATHING

#define A5_AUDIO
#define AUDIO_PIN A5

#define STARTUP_SONG SONG(NOCTURNE_OP_9_NO_1)
