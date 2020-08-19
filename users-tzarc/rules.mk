SRC += \
	tzarc_common.c \
	tzarc_eeprom.c \
	tzarc_rawhid.c \
	tzarc_wow.c \
	tzarc_diablo3.c

BOOTMAGIC_ENABLE = lite
UNICODE_ENABLE = yes
LTO_ENABLE = yes
RAW_ENABLE = no

# board-specific configs
ifeq ($(strip $(KEYBOARD)), massdrop/ctrl)
	LTO_ENABLE = no
endif

# platform-specific configs
ifeq ($(OPT_OS),chibios)
	RAW_ENABLE = yes
endif
