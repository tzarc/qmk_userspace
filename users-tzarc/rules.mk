SRC += \
	tzarc_common.c \
	tzarc_eeprom.c \
	tzarc_rawhid.c \
	tzarc_wow.c \
	tzarc_diablo3.c

UNICODE_ENABLE = yes
LTO_ENABLE = yes

# platform-specific configs
ifeq ($(PLATFORM),CHIBIOS)
    RAW_ENABLE = yes
endif

# board-specific configs
ifeq ($(strip $(KEYBOARD)), massdrop/ctrl)
    LTO_ENABLE = no
endif
