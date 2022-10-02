# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later

DEBUG_MATRIX_SCAN_RATE_ENABLE = api

#QUANTUM_PAINTER_DRIVERS += rgb565_surface
#QUANTUM_PAINTER_LVGL_INTEGRATION = yes

LUA_ENABLE = yes
MOUSEKEY_ENABLE = yes
UNICODE_ENABLE = yes

CONSOLE_ENABLE = yes

ENCODER_MAP_ENABLE = yes

SWAP_HANDS_ENABLE = no

AUDIO_ENABLE = no

VPATH += keyboards/tzarc/djinn/graphics
SRC += \
	djinn.qgf.c \
	lock-caps-ON.qgf.c \
	lock-scrl-ON.qgf.c \
	lock-num-ON.qgf.c \
	lock-caps-OFF.qgf.c \
	lock-scrl-OFF.qgf.c \
	lock-num-OFF.qgf.c \
	thintel15.qff.c

#EEPROM_DRIVER = wear_leveling
#WEAR_LEVELING_DRIVER = embedded_flash

#EEPROM_DRIVER = wear_leveling
#WEAR_LEVELING_DRIVER = spi_flash

#OPT = 1
#OPT_DEFS += -g
