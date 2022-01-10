// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// Split configuration
#define SERIAL_USART_DRIVER SD3
#define SERIAL_USART_TX_PAL_MODE 7
#define SOFT_SERIAL_PIN B9
#define SERIAL_USART_SPEED 640000
#define SPLIT_HAND_PIN B11

// RGB configuration
#define RGB_POWER_ENABLE_PIN B1
#define RGB_CURR_1500mA_OK_PIN B0
#define RGB_CURR_3000mA_OK_PIN C5
#define RGBLED_NUM 84
#define RGBLED_SPLIT \
    { 42, 42 }

// EEPROM configuration
#define EXTERNAL_EEPROM_SPI_SLAVE_SELECT_PIN B5
#define EXTERNAL_EEPROM_SPI_CLOCK_DIVISOR 32
#define EXTERNAL_EEPROM_BYTE_COUNT 4096
#define EXTERNAL_EEPROM_PAGE_SIZE 64
