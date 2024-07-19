# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

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

XAP_ENABLE ?= no
RAW_ENABLE ?= no
VIA_ENABLE ?= no
KONAMI_CODE_ENABLE ?= no
REPEAT_KEY_ENABLE ?= yes

ifeq ($(strip $(PLATFORM_KEY)),chibios)
	CREATE_MAP = yes
	EXTRAFLAGS = -fstack-usage
	EXTRALDFLAGS = -Wl,--print-memory-usage
	DEBOUNCE_TYPE = asym_eager_defer_pk
	KONAMI_CODE_ENABLE ?= yes

else ifeq ($(strip $(PLATFORM_KEY)),arm_atsam)
	LTO_ENABLE = no
else ifeq ($(strip $(PLATFORM_KEY)),avr)
	GAME_MODES_ENABLE = no
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

ifeq ($(strip $(RGB_MATRIX_ENABLE)),yes)
	RGB_MATRIX_CUSTOM_USER = yes
	RV32_VM_ENABLE = yes
	SRC += rgb_matrix_rv32_runner.c

generated-files: $(QMK_USERSPACE)/users/tzarc/rv32_rgb_runner.inl.h

$(QMK_USERSPACE)/users/tzarc/rv32_rgb_runner.inl.h:
	@$(MAKE) -C $(QMK_USERSPACE)/users/tzarc/rv32_rgb_runner
	[ ! -f $(QMK_USERSPACE)/users/tzarc/rv32_rgb_runner.inl.h ] || rm -f $(QMK_USERSPACE)/users/tzarc/rv32_rgb_runner.inl.h
	@xxd -n rv32_runner -i $(QMK_USERSPACE)/users/tzarc/rv32_rgb_runner/rv32_runner.bin $(QMK_USERSPACE)/users/tzarc/rv32_rgb_runner.inl.h

endif


include $(QMK_USERSPACE)/qmk_firmware-mods/builddefs/common_features_mod.mk
