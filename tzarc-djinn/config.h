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
    { C5, C4, A10 }
#define MATRIX_COL_PINS \
    { A9, B5, B10 }

#define DIODE_DIRECTION ROW2COL

// Matrix debugging
#define DEBUG_MATRIX_SCAN_RATE

// Bootloader
#define STM32_BOOTLOADER_DUAL_BANK TRUE
#define STM32_BOOTLOADER_DUAL_BANK_GPIO B0

// USB Device parameters
#define VENDOR_ID 0xF055
#define PRODUCT_ID 0x4920
#define DEVICE_VER 0x0001
#define MANUFACTURER Tzarc
#define PRODUCT Djinn
#define DESCRIPTION Oversized Split

// Power control pins
#define LCD_POWER_ENABLE_PIN A1
#define RGB_POWER_ENABLE_PIN A0

// SPI Configuration
#define SPI_DRIVER SPID3
#define SPI_SCK_PIN C10
#define SPI_SCK_PAL_MODE 6
#define SPI_MOSI_PIN C12
#define SPI_MOSI_PAL_MODE 6
#define SPI_MISO_PIN C11
#define SPI_MISO_PAL_MODE 6

// LCD Configuration
#define ILI9341_SPI_DIVISOR 16
#define ILI9341_PIXDATA_BUFSIZE 240
#define LCD_RST_PIN B3
#define LCD_CS_PIN D2
#define LCD_DC_PIN A15

// Backlight driver (to control LCD backlight)
#define BACKLIGHT_LEVELS 4
#define BACKLIGHT_PIN A7
#define BACKLIGHT_PWM_DRIVER PWMD17
#define BACKLIGHT_PWM_CHANNEL 1
#define BACKLIGHT_PAL_MODE 1
#define BACKLIGHT_BREATHING

// RGB configuration
#define STM32_DMA_REQUIRED  // ws2812 PWM driver doesn't trigger this apparently
#define RGB_DI_PIN B2
#define RGBLED_NUM 6
#define WS2812_PWM_DRIVER PWMD20
#define WS2812_PWM_CHANNEL 1
#define WS2812_PWM_PAL_MODE 3
#define WS2812_DMA_STREAM STM32_DMA1_STREAM1
#define WS2812_DMA_CHANNEL 1
#define WS2812_DMAMUX_ID STM32_DMAMUX1_TIM20_UP
#define DRIVER_LED_TOTAL RGBLED_NUM
#define RGBLIGHT_ANIMATIONS
#define RGBLIGHT_LIMIT_VAL 100
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 100
#define RGB_MATRIX_KEYPRESSES
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#define NOP_FUDGE 0.42

// Audio configuration
#define AUDIO_PIN A5
#define AUDIO_PIN_ALT A4
#define AUDIO_PIN_ALT_AS_NEGATIVE
#define A5_AUDIO
#define STARTUP_SONG SONG(NOCTURNE_OP_9_NO_1)

// Display configuration
#define QUANTUM_PAINTER_COMPRESSED_CHUNK_SIZE 4096

/* disable action features */
#define NO_ACTION_LAYER
#define NO_ACTION_TAPPING
#define NO_ACTION_ONESHOT

/* disable these deprecated features by default */
#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION
