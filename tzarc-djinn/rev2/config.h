// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// Split configuration
#define SPLIT_HAND_PIN B9

#if defined(SERIAL_DRIVER_USART)
#    define SERIAL_USART_DRIVER SD3
#    define SERIAL_USART_PIN_SWAP
#    define SERIAL_USART_TX_PIN B10
#    define SERIAL_USART_TX_PAL_MODE 7
#    define SERIAL_USART_RX_PIN B11
#    define SERIAL_USART_RX_PAL_MODE 7
#    ifndef SERIAL_USART_SPEED
#        define SERIAL_USART_SPEED 1200000
#    endif // SERIAL_USART_SPEED
#    define SERIAL_USART_FULL_DUPLEX
#endif // defined(SERIAL_DRIVER_USART)

// RGB configuration
#define RGB_POWER_ENABLE_PIN B0
#define RGB_CURR_1500mA_OK_PIN C5
#define RGB_CURR_3000mA_OK_PIN C4
#define RGBLED_NUM 86
#define RGBLED_SPLIT \
    { 43, 43 }

// EEPROM configuration
#define EXTERNAL_EEPROM_SPI_SLAVE_SELECT_PIN B5
#define EXTERNAL_EEPROM_SPI_CLOCK_DIVISOR 8 // (160MHz/8) => 20MHz
#define EXTERNAL_EEPROM_BYTE_COUNT 8192
#define EXTERNAL_EEPROM_PAGE_SIZE 64 // it's FRAM, so it doesn't actually matter, this just sets the RAM buffer

// External flash config
#define EXTERNAL_FLASH_SPI_MODE 3
#define EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN A8
#define EXTERNAL_FLASH_SPI_CLOCK_DIVISOR 4    // (160MHz/4) => 40MHz
#define EXTERNAL_FLASH_SIZE (4 * 1024 * 1024) // 32Mb/4MB capacity
#define DEBUG_FLASH_SPI_OUTPUT

// Fault indicators
#define BOARD_POWER_FAULT_PIN C9
#define RGB_POWER_FAULT_PIN C4

// Limit the backlight brightness
#define BACKLIGHT_LIMIT_VAL 144