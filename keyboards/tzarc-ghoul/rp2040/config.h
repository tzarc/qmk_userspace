// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "config_common.h"

// Change PRODUCT based on hosted MicroMod
#define PRODUCT Ghoul_RP2040

// Matrix configuration
#define SPI_MATRIX_CHIP_SELECT_PIN GP21
#define SPI_MATRIX_DIVISOR 16

// Encoder
#define ENCODERS_PAD_A \
    { GP8 }
#define ENCODERS_PAD_B \
    { GP5 }
#define ENCODER_PUSHBUTTON_PIN GP7

// SPI Configuration
#define SPI_DRIVER SPID0
#define SPI_SCK_PIN GP22
#define SPI_MOSI_PIN GP23
#define SPI_MISO_PIN GP20

// EEPROM configuration
#define EXTERNAL_EEPROM_SPI_SLAVE_SELECT_PIN GP3

// RGB configuration
#define RGB_DI_PIN GP13
#define RGB_ENABLE_PIN GP6

// ADC Configuration
#define ADC_RESOLUTION ADC_CFGR1_RES_12BIT
#define ADC_SATURATION ((1 << 12) - 1)
#define ADC_CURRENT_PIN GP26
#define ADC_VOLTAGE_PIN GP27

// Display Configuration
#define OLED_CS_PIN GP16
#define OLED_DC_PIN GP17
#define OLED_RST_PIN GP18
