MCU = cortex-m4
ARMV = 7
MCU_FAMILY = STM32
MCU_SERIES = STM32G4xx
MCU_LDSCRIPT = STM32G474xE
MCU_STARTUP = stm32g4xx
BOARD = ST_NUCLEO64_G474RE
USE_FPU = YES
DFU_ARGS = -d 0483:df11 -a 0 -s 0x08000000:leave
DFU_SUFFIX_ARGS = -v 0483 -p df11
#STM32_BOOTLOADER_ADDRESS = 0x1FFF0000

#-------------------------------------------------

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = lite     # Virtual DIP switch configuration
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = yes        # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend
NKRO_ENABLE = no            # USB Nkey Rollover
BACKLIGHT_ENABLE = yes      # Enable keyboard backlight functionality on B7 by default
RGBLIGHT_ENABLE = yes       # Enable keyboard RGB underglow
UNICODE_ENABLE = no         # Unicode
AUDIO_ENABLE = no           # Audio output

ENCODER_ENABLE = yes

# Allow for single-side builds to be overridden in keymaps
SPLIT_KEYBOARD ?= yes

ifeq ($(strip $(SPLIT_KEYBOARD)),yes)
	SERIAL_DRIVER = usart_statesync
endif

BACKLIGHT_DRIVER = pwm

WS2812_DRIVER = pwm
CIE1931_CURVE = yes

#RGB_MATRIX_ENABLE = WS2812

EEPROM_DRIVER = spi

AUDIO_DRIVER = pwm_software
AUDIO_PIN = A5
AUDIO_PIN_ALT = A4

QUANTUM_PAINTER_DRIVERS = ili9341

# Disable LTO as it messes with data transactions between sides
LTO_ENABLE = no
OPT = 2

ENABLE_ADC_USBPD_CHECK = no

ifeq ($(strip $(ENABLE_ADC_USBPD_CHECK)), yes)
	OPT_DEFS += -DENABLE_ADC_USBPD_CHECK
	SRC += analog.c
endif

#LTO_ENABLE = no
#OPT = 0
#OPT_DEFS += -g