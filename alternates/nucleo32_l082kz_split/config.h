// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
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
#define SERIAL_USART_SPEED 500000 // 9600 // 38400 // 115200 // 230400 // 460800 // 921600 // 1843200 // 2000000

#define DEBUG_MATRIX_SCAN_RATE

#define MATRIX_IO_DELAY 10
