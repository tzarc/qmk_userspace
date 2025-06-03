// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#define BACKLIGHT_PWM_DRIVER PWMD5
#define BACKLIGHT_PWM_CHANNEL 1

#define ADC_PIN A0

#define SOLENOID_PIN B12
#define SOLENOID_PINS {B12, B13, B14, B15}
#define SOLENOID_PINS_ACTIVE_STATE {high, high, low}

#define SPI_DRIVER SPID1
#define SPI_SCK_PIN A5
#define SPI_MOSI_PIN A7
#define SPI_MISO_PIN A6
#define SPI_SCK_PAL_MODE 5
#define SPI_MOSI_PAL_MODE 5
#define SPI_MISO_PAL_MODE 5

#define EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN A4

#define EEPROM_SIZE 2048
