# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

CUSTOM_MATRIX = lite
EEPROM_DRIVER = spi
RGB_MATRIX_DRIVER = WS2812
QUANTUM_PAINTER_DRIVERS = ssd1351_spi
OPT_DEFS += -DCORTEX_ENABLE_WFI_IDLE=TRUE
