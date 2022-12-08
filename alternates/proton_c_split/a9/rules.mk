# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

# MCU name
MCU = STM32F303
CONSOLE_ENABLE = yes

SERIAL_DRIVER = usart_duplex

RGBLIGHT_ENABLE = yes
WS2812_DRIVER = pwm

#BACKLIGHT_ENABLE = no
#BACKLIGHT_DRIVER = pwm

AUDIO_ENABLE = no
AUDIO_DRIVER = pwm_hardware
