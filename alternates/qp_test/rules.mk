# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
QUANTUM_PAINTER_DRIVERS = \
    surface \
    ili9163_spi \
    ili9341_spi \
    ili9488_spi \
    st7735_spi \
    st7789_spi \
    gc9a01_spi \
    ssd1351_spi \
    sh1106_i2c \
    sh1106_spi

#OPT = 0
#OPT_DEFS += -g

DEBUG_MATRIX_SCAN_RATE_ENABLE = yes

OPT = 3
LTO_ENABLE = yes