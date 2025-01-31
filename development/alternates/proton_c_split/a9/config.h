// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "config_common.h"

#define EARLY_INIT_PERFORM_BOOTLOADER_JUMP TRUE

#define SERIAL_BUFFERS_SIZE 1024

#define MATRIX_ROW_PINS {A13}
#define MATRIX_COL_PINS {A14}
#define UNUSED_PINS

#ifdef SERIAL_DRIVER_USART
#    define SOFT_SERIAL_PIN A9
#    define SERIAL_USART_SPEED 460800
#endif // SERIAL_DRIVER_USART

#ifdef SERIAL_DRIVER_USART_DUPLEX
#    define SERIAL_USART_DRIVER UARTD1
#    define SERIAL_USART_PIN_SWAP
#    define SERIAL_USART_TX_PIN A9
#    define SERIAL_USART_RX_PIN A10
#endif // SERIAL_DRIVER_USART_DUPLEX

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
