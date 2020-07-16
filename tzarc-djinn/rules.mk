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

#-------------------------------------------------

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = no       # Virtual DIP switch configuration
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = yes        # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend
NKRO_ENABLE = yes           # USB Nkey Rollover
BACKLIGHT_ENABLE = yes      # Enable keyboard backlight functionality on B7 by default
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
MIDI_ENABLE = no            # MIDI support
UNICODE_ENABLE = no         # Unicode
BLUETOOTH_ENABLE = no       # Enable Bluetooth with the Adafruit EZ-Key HID
AUDIO_ENABLE = no           # Audio output
FAUXCLICKY_ENABLE = no      # Use buzzer to emulate clicky switches
HD44780_ENABLE = no         # Enable support for HD44780 based LCDs

BACKLIGHT_DRIVER = pwm

WS2812_DRIVER = pwm
CIE1931_CURVE = yes

RGBLIGHT_ENABLE = yes
#RGB_MATRIX_ENABLE = WS2812

VPATH += \
	drivers/quantum_painter \
	drivers/quantum_painter/3rdparty \
	drivers/quantum_painter/img \
	drivers/quantum_painter/lld/common \
	drivers/quantum_painter/lld/ili9341

SRC += \
	spi_master.c \
	lzf_d.c \
	qp_common.c \
	qp_utils.c \
	qp_ili9341.c
