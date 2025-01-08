// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

// Configurables
#define RGB_ENABLE_PIN C11

// WS2812 Configuration
#define WS2812_PWM_DRIVER PWMD17
#define WS2812_PWM_CHANNEL 1
#define WS2812_PWM_PAL_MODE 10
#define WS2812_PWM_DMA_STREAM STM32_DMA1_STREAM1
#define WS2812_PWM_DMA_CHANNEL 1
#define WS2812_PWM_DMAMUX_ID STM32_DMAMUX1_TIM17_UP

// SPI Configuration
#define SPI_DRIVER SPID1
#define SPI_SCK_PIN A5
#define SPI_SCK_PAL_MODE 5
#define SPI_MOSI_PIN A7
#define SPI_MOSI_PAL_MODE 5
#define SPI_MISO_PIN A6
#define SPI_MISO_PAL_MODE 5

// External NOR Flash config
#define EXTERNAL_FLASH_SPI_MODE 0
#define EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN B10
#define EXTERNAL_FLASH_SPI_CLOCK_DIVISOR 4     // (160MHz/4) => 40MHz
#define EXTERNAL_FLASH_SIZE (16 * 1024 * 1024) // 128Mb/16MB capacity

// I2C Configuration (@1MHz)
#define I2C_DRIVER I2CD1
#define I2C1_SCL_PIN A15
#define I2C1_SCL_PAL_MODE 4
#define I2C1_SDA_PIN B7
#define I2C1_SDA_PAL_MODE 4
#define I2C1_TIMINGR_PRESC 0b0001
#define I2C1_TIMINGR_SCLDEL 0b1011
#define I2C1_TIMINGR_SDADEL 0b0011
#define I2C1_TIMINGR_SCLH 0b00001111
#define I2C1_TIMINGR_SCLL 0b00100101
