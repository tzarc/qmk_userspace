# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

# Ensure file listings are generated
# cat .build/obj_tzarc_djinn_rev2_tzarc/tzarc_unicode.i | sed -e 's@^#.*@@g' -e 's@^\s*//.*@@g' -e '/^\s*$/d' | clang-format
OPT_DEFS += -save-temps=obj

TOP_SYMBOLS = 80
NKRO_ENABLE ?= no
COMMAND_ENABLE = no
SPACE_CADET_ENABLE ?= no
MAGIC_ENABLE ?= no
CONSOLE_ENABLE ?= yes
LTO_ENABLE := no
DEBUG_MATRIX_SCAN_RATE_ENABLE ?= yes
BOOTMAGIC_ENABLE ?= yes
EXTRAKEY_ENABLE ?= yes
MOUSEKEY_ENABLE ?= yes
RAW_ENABLE ?= no
UNICODE_ENABLE ?= yes
DEFERRED_EXEC_ENABLE = yes
GAME_MODES_ENABLE ?= yes

DIGITIZER_ENABLE = yes
PROGRAMMABLE_BUTTON_ENABLE = yes

XAP_ENABLE ?= no
RAW_ENABLE ?= no
VIA_ENABLE ?= no
KONAMI_CODE_ENABLE ?= no
REPEAT_KEY_ENABLE ?= yes

EXTRALDFLAGS = -Xlinker -Map=$(BUILD_DIR)/$(TARGET).map

ifeq ($(strip $(PLATFORM_KEY)),chibios)
	OPT ?= 2
	EXTRAFLAGS = -fstack-usage
	EXTRALDFLAGS += -Wl,--print-memory-usage
	DEBOUNCE_TYPE = asym_eager_defer_pk
	KONAMI_CODE_ENABLE ?= yes

else ifeq ($(strip $(PLATFORM_KEY)),avr)
	OPT ?= s
	GAME_MODES_ENABLE = no
    UNICODE_ENABLE = no
	ifeq ($(strip $(PROTOCOL)),LUFA)
		# Uses defaults as per rules.mk
	else ifeq ($(strip $(PROTOCOL)),VUSB)
		CONSOLE_ENABLE = no
		DEBUG_MATRIX_SCAN_RATE_ENABLE = no
	endif
endif

SRC += \
	tzarc_common.c \
	tzarc_names.c \
	tzarc_eeprom.c

ifeq ($(strip $(QUANTUM_PAINTER_ENABLE)),yes)
	VPATH += $(USER_PATH)/graphics/src
	SRC += tzarc_screen.c
endif

ifeq ($(strip $(UNICODE_ENABLE)),yes)
	SRC += tzarc_unicode.c
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

#OPT_DEFS += -fsanitize=undefined
