# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
SRC += \
	tzarc_common.c \
	tzarc_keynames.c \
	tzarc_eeprom.c \
	tzarc_unicode.c \
	tzarc_screen.c

# Ensure file listings are generated
# cat .build/obj_tzarc_djinn_rev2_tzarc/tzarc_unicode.i | sed -e 's@^#.*@@g' -e 's@^\s*//.*@@g' -e '/^\s*$/d' | clang-format
OPT_DEFS += -save-temps=obj

TOP_SYMBOLS = 25
NKRO_ENABLE ?= no
COMMAND_ENABLE = no
SPACE_CADET_ENABLE ?= no
MAGIC_ENABLE ?= no
CONSOLE_ENABLE ?= yes
LTO_ENABLE ?= yes
DEBUG_MATRIX_SCAN_RATE_ENABLE ?= yes
BOOTMAGIC_ENABLE ?= yes
EXTRAKEY_ENABLE ?= yes
MOUSEKEY_ENABLE ?= yes
RAW_ENABLE ?= no
UNICODE_ENABLE ?= yes
DEFERRED_EXEC_ENABLE = yes
GAME_MODES_ENABLE ?= yes

VPATH += $(USER_PATH)/graphics/src

XAP_ENABLE ?= no
RAW_ENABLE ?= no
VIA_ENABLE ?= no
KONAMI_CODE_ENABLE ?= no

ifeq ($(strip $(PLATFORM_KEY)),chibios)
	CREATE_MAP = yes
	EXTRAFLAGS = -fstack-usage
	EXTRALDFLAGS = -Wl,--print-memory-usage
	DEBOUNCE_TYPE = asym_eager_defer_pk
	KONAMI_CODE_ENABLE = yes
else ifeq ($(strip $(PLATFORM_KEY)),arm_atsam)
	LTO_ENABLE = no
else ifeq ($(strip $(PLATFORM_KEY)),avr)
	ifeq ($(strip $(PROTOCOL)),LUFA)
		# Uses defaults as per rules.mk
	else ifeq ($(strip $(PROTOCOL)),VUSB)
		CONSOLE_ENABLE = no
		DEBUG_MATRIX_SCAN_RATE_ENABLE = no
	endif
endif

ifneq ("$(wildcard $(LIB_PATH)/lvgl)","")

#	ifeq ($(strip $(QUANTUM_PAINTER_LVGL_INTEGRATION)),yes)
#		VPATH += $(USER_PATH)/screens/diablo
#		SRC += ui.c ui_helpers.c ui_img_d3_png.c
#	endif

endif

ifeq ($(strip $(KONAMI_CODE_ENABLE)),yes)
	OPT_DEFS += -DKONAMI_CODE_ENABLE
endif

ifeq ($(strip $(GAME_MODES_ENABLE)),yes)
	OPT_DEFS += -DGAME_MODES_ENABLE

	SRC += \
		tzarc_wow.c \
		tzarc_diablo.c
endif

#OPT = 0