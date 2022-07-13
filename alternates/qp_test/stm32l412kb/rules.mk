# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later

MCU = STM32L412
BOOTLOADER = stm32-dfu

EEPROM_DRIVER = wear_leveling
WEAR_LEVELING_DRIVER = embedded_flash

OPT = 1
OPT_DEFS += -g
