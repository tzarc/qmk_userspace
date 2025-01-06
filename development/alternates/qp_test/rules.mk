# Copyright 2018-2025 Nick Brassel (@tzarc)
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

DEBOUNCE_TYPE = asym_eager_defer_pk

OPT = 0
OPT_DEFS += -g

DEBUG_MATRIX_SCAN_RATE_ENABLE = yes

TOP_SYMBOLS=3000

OPT = 3
LTO_ENABLE = yes
