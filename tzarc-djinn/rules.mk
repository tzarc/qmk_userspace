MCU = STM32G474
BOOTLOADER = stm32-dfu

SRC += \
	djinn_portscan_matrix.c \
	djinn_split_sync.c \
	djinn_usbpd.c

BOOTMAGIC_ENABLE = yes      # Virtual DIP switch configuration
MOUSEKEY_ENABLE = no        # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend
NKRO_ENABLE = no            # USB Nkey Rollover
UNICODE_ENABLE = no         # Unicode
WPM_ENABLE = yes

SPLIT_KEYBOARD = yes

ENCODER_ENABLE = yes

USBPD_ENABLE = yes

BACKLIGHT_ENABLE = yes
BACKLIGHT_DRIVER = pwm

WS2812_DRIVER = pwm
CIE1931_CURVE = yes

RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = WS2812

EEPROM_DRIVER = spi

AUDIO_ENABLE = yes
AUDIO_DRIVER = pwm_software
AUDIO_PIN = A5
AUDIO_PIN_ALT = A4

QUANTUM_PAINTER_ENABLE = yes
QUANTUM_PAINTER_DRIVERS = ili9341_spi

USE_FPU = yes
LTO_ENABLE = yes
OPT = 2

DEFAULT_FOLDER = tzarc/djinn/rev2