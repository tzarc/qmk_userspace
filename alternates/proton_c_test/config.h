// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "config_common.h"

#define MATRIX_COL_PINS \
    { A13 }
#define MATRIX_ROW_PINS \
    { A14 }
#define UNUSED_PINS

#define DEBUG_MATRIX_SCAN_RATE

#define MATRIX_IO_DELAY 10

#define RGB_DI_PIN B9
#define WS2812_PWM_DRIVER PWMD4
#define WS2812_PWM_CHANNEL 4
#define WS2812_PWM_PAL_MODE 2
#define WS2812_DMA_CHANNEL 7
#define RGBLIGHT_LIMIT_VAL 100

#define BACKLIGHT_PIN B8
#define BACKLIGHT_PWM_DRIVER PWMD4
#define BACKLIGHT_PWM_CHANNEL 3
#define BACKLIGHT_PAL_MODE 2

#define BACKLIGHT_BREATHING

#define AUDIO_PIN A5
#define AUDIO_PIN_ALT A4
#define AUDIO_PIN_ALT_AS_NEGATIVE

#define STARTUP_SONG SONG(NOCTURNE_OP_9_NO_1)

#define EXTERNAL_EEPROM_SPI_SLAVE_SELECT_PIN B8
#define EXTERNAL_EEPROM_SPI_CLOCK_DIVISOR 4
#define EXTERNAL_EEPROM_BYTE_COUNT 32768
