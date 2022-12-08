// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "config_common.h"

#define EARLY_INIT_PERFORM_BOOTLOADER_JUMP TRUE

#define HAL_USE_SERIAL TRUE
#define SERIAL_BUFFERS_SIZE 1024

#define MATRIX_ROW_PINS \
    { B5 }
#define MATRIX_COL_PINS \
    { B4 }
#define UNUSED_PINS

#define SOFT_SERIAL_PIN A9
// #define SOFT_SERIAL_PIN_2 A10

#define SERIAL_USART_SPEED 2000000 // 9600 // 38400 // 115200 // 230400 // 460800 // 921600 // 1843200 // 2000000

#define SPLIT_USB_DETECT
#define SPLIT_HAND_PIN A2

#define DEBUG_MATRIX_SCAN_RATE

// #define MATRIX_IO_DELAY 10

#define WS2812_EXTERNAL_PULLUP
#define RGB_DI_PIN B9
#define WS2812_PWM_DRIVER PWMD4
#define WS2812_PWM_CHANNEL 4
#define WS2812_PWM_PAL_MODE 2
#define WS2812_DMA_STREAM STM32_DMA1_STREAM6
#define WS2812_DMA_CHANNEL 2
#define RGBLIGHT_LIMIT_VAL 100

#define BACKLIGHT_PIN B8
#define BACKLIGHT_PWM_DRIVER PWMD4
#define BACKLIGHT_PWM_CHANNEL 3
#define BACKLIGHT_PAL_MODE 2

#define BACKLIGHT_BREATHING

#define A5_AUDIO
#define AUDIO_PIN A5

#define STARTUP_SONG SONG(NOCTURNE_OP_9_NO_1)
