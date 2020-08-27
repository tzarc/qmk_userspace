SRC += \
	tzarc_common.c \
	tzarc_eeprom.c \
	tzarc_rawhid.c \
	tzarc_wow.c \
	tzarc_diablo3.c

NKRO_ENABLE = yes
EXTRAKEY_ENABLE = yes
KEYBOARD_SHARED_EP = yes
MOUSEKEY_ENABLE = no
CONSOLE_ENABLE = yes
BOOTMAGIC_ENABLE = lite
UNICODE_ENABLE = yes
LTO_ENABLE = yes
RAW_ENABLE = yes

ifeq ($(strip $(PLATFORM)),CHIBIOS)
	# Uses defaults above
else ifeq ($(strip $(PLATFORM)),ARM_ARSAM)
	# This shit's broken, surprise surprise.
	LTO_ENABLE = no
	RAW_ENABLE = no
else ifeq ($(strip $(PLATFORM)),AVR)
	CONSOLE_ENABLE = no
	RAW_ENABLE = no
	ifeq ($(strip $(PROTOCOL)),LUFA)
		# Empty atm
	else ifeq ($(strip $(PROTOCOL)),VUSB)
		# Empty atm
	endif
endif