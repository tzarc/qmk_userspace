# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

DEBUG_MATRIX_SCAN_RATE_ENABLE = api

QUANTUM_PAINTER_DRIVERS += surface
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

#####################

RGB_MATRIX_RV32 = yes

ifneq ($(strip $(KEYBOARD)),tzarc/djinn/rev1)

FLASH_DRIVER = spi
FILESYSTEM_DRIVER = lfs_flash

VPATH += \
		$(QMK_USERSPACE)/qmk_firmware-mods \
		$(QMK_USERSPACE)/qmk_firmware-mods/quantum \
		$(QMK_USERSPACE)/qmk_firmware-mods/quantum/filesystem

#EEPROM_DRIVER = custom
#SRC += eeprom_filesystem.c
VPATH += \
		$(QMK_USERSPACE)/qmk_firmware-mods/drivers/eeprom

endif
