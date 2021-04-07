/* Copyright 2021 Nick Brassel (@tzarc)
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

// Split configuration
#define SERIAL_USART_DRIVER SD3
#define SERIAL_USART_TX_PAL_MODE 7
#define SOFT_SERIAL_PIN B10
//#define SOFT_SERIAL_PIN_ALT B11
#define SERIAL_USART_SPEED 1811200
#define SPLIT_HAND_PIN B9

// RGB configuration
#define RGB_POWER_ENABLE_PIN B0
#define RGB_CURR_1500mA_OK_PIN C5
#define RGB_CURR_3000mA_OK_PIN C4
#define RGBLED_NUM 86
#define RGBLED_SPLIT \
    { 43, 43 }

// External flash config
#define EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN A8

// Fault indicators
#define BOARD_POWER_FAULT_PIN C9
#define RGB_POWER_FAULT_PIN C4
