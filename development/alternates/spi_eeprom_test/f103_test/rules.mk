# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

# MCU name
MCU = STM32F103
BOARD = STM32_F103_STM32DUINO


CONSOLE_ENABLE = yes

RGBLIGHT_ENABLE = no
#WS2812_DRIVER = pwm

BACKLIGHT_ENABLE = no
#BACKLIGHT_DRIVER = pwm

AUDIO_ENABLE = no
#AUDIO_DRIVER = dac

EEPROM_DRIVER = spi

SRC += f103.c
