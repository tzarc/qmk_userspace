// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "config_common.h"

// USB Device parameters
#define VENDOR_ID 0x1209
#define PRODUCT_ID 0x4920
#define DEVICE_VER 0x0001
#define MANUFACTURER Tzarc

// Matrix
#define MATRIX_SHIFT_REGISTER_COUNT 5
#define MATRIX_ROWS 6 // 5 shift registers, plus one row for extras (i.e. encoder pushbutton read)
#define MATRIX_COLS 8 // 8 bits per register

// Encoder
#ifndef ENCODER_RESOLUTION
#    define ENCODER_RESOLUTION 2
#endif // ENCODER_RESOLUTION

// EEPROM configuration
#define EXTERNAL_EEPROM_SPI_CLOCK_DIVISOR 16 // (168MHz/16) => 10.5MHz
#define EXTERNAL_EEPROM_BYTE_COUNT 8192
#define EXTERNAL_EEPROM_PAGE_SIZE 64 // it's FRAM, so it doesn't actually matter, this just sets the RAM buffer

// RGB configuration
#define RGBLED_NUM 40
#define DRIVER_LED_TOTAL RGBLED_NUM

//!! TEMPORARY UNTIL RGB_MATRIX
#define RGBLIGHT_ANIMATIONS
#define RGBLIGHT_EFFECT_BREATHING
#define RGBLIGHT_EFFECT_RAINBOW_MOOD
#define RGBLIGHT_EFFECT_RAINBOW_SWIRL
#define RGBLIGHT_EFFECT_SNAKE
#define RGBLIGHT_EFFECT_KNIGHT
#define RGBLIGHT_EFFECT_CHRISTMAS
#define RGBLIGHT_EFFECT_STATIC_GRADIENT
#define RGBLIGHT_EFFECT_RGB_TEST
#define RGBLIGHT_EFFECT_ALTERNATING
#define RGBLIGHT_EFFECT_TWINKLE
