# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

# MCU name
MCU = STM32F303
CONSOLE_ENABLE = yes

RGBLIGHT_ENABLE = no
#WS2812_DRIVER = pwm

BACKLIGHT_ENABLE = no
#BACKLIGHT_DRIVER = pwm

AUDIO_ENABLE = no
AUDIO_DRIVER = dac_basic

EEPROM_DRIVER = spi

SRC += proton_c.c
