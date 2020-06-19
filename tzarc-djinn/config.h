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

// Matrix
#define MATRIX_ROWS 3
#define MATRIX_COLS 3
#define MATRIX_ROW_PINS \
    { B8, B9, A5 }
#define MATRIX_COL_PINS \
    { A10, C4, C5 }

#define DIODE_DIRECTION ROW2COL

// Matrix debugging
#define DEBUG_MATRIX_SCAN_RATE

// Bootloader
//#define EARLY_INIT_PERFORM_BOOTLOADER_JUMP TRUE
//#define STM32_BOOTLOADER_DUAL_BANK TRUE
//#define STM32_BOOTLOADER_DUAL_BANK_GPIO B0

// USB Device parameters
#define VENDOR_ID 0xF055
#define PRODUCT_ID 0x4920
#define DEVICE_VER 0x0001
#define MANUFACTURER Tzarc
#define PRODUCT Djinn
#define DESCRIPTION Oversized Split

// Power control pins
#define LCD_POWER_ENABLE_PIN A4
#define RGB_POWER_ENABLE_PIN A0

// Backlight driver (to control LCD backlight)
#define BACKLIGHT_LEVELS 4
#define BACKLIGHT_PIN C0
#define BACKLIGHT_PWM_DRIVER PWMD1
#define BACKLIGHT_PWM_CHANNEL 1
#define BACKLIGHT_PAL_MODE 2
#define BACKLIGHT_BREATHING

// RGB configuration
#define STM32_DMA_REQUIRED  // ws2812 PWM driver doesn't trigger this apparently
#define RGB_DI_PIN A6
#define RGBLED_NUM 6
#define WS2812_PWM_DRIVER PWMD3
#define WS2812_PWM_CHANNEL 1
#define WS2812_PWM_PAL_MODE 2
#define WS2812_DMA_CHANNEL STM32_DMA_STREAM_ID_ANY
#define WS2812_DMAMUX_ID STM32_DMAMUX1_TIM3_UP
#define DRIVER_LED_TOTAL RGBLED_NUM
#define RGBLIGHT_ANIMATIONS
#define RGBLIGHT_LIMIT_VAL 120
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 120
#define RGB_MATRIX_KEYPRESSES
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#define NOP_FUDGE 0.42
