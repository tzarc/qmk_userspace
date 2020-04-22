SRC += \
	tzarc_common.c \
	tzarc_eeprom.c \
	tzarc_wow.c \
	tzarc_diablo3.c

UNICODE_COMMON = yes
LTO_ENABLE = yes

# board-specific configs
ifeq ($(strip $(KEYBOARD)), massdrop/ctrl)
    LTO_ENABLE = no
endif
