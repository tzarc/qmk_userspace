SRC += \
	tzarc_common.c \
	tzarc_eeprom.c \
	tzarc_rawhid.c \
	tzarc_wow.c \
	tzarc_diablo3.c

NKRO_ENABLE = no
COMMAND_ENABLE = no
SPACE_CADET_ENABLE = no
MAGIC_ENABLE = no
LTO_ENABLE ?= yes
DEBUG_MATRIX_SCAN_RATE_ENABLE ?= yes
BOOTMAGIC_ENABLE ?= lite
EXTRAKEY_ENABLE ?= yes
MOUSEKEY_ENABLE ?= yes
RAW_ENABLE ?= no
UNICODE_ENABLE ?= yes

VPATH += $(USER_PATH)/graphics/src
