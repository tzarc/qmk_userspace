MCU = STM32G474

BOOTMAGIC_ENABLE = lite     # Virtual DIP switch configuration
MOUSEKEY_ENABLE = no        # Mouse keys
EXTRAKEY_ENABLE = no        # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend
NKRO_ENABLE = no            # USB Nkey Rollover
BACKLIGHT_ENABLE = yes      # Enable keyboard backlight functionality on B7 by default
RGBLIGHT_ENABLE = yes       # Enable keyboard RGB underglow
UNICODE_ENABLE = no         # Unicode
AUDIO_ENABLE = yes          # Audio output

ENCODER_ENABLE = yes
USBPD_ENABLE = yes

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

QUANTUM_PAINTER_ENABLE = yes
QUANTUM_PAINTER_DRIVERS = rgb565_surface ili9341

LTO_ENABLE = yes
OPT = 2

#LTO_ENABLE = no
#OPT = 0
#OPT_DEFS += -g

LUA_ENABLE = yes
