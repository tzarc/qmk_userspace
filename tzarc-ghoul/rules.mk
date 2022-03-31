BOOTMAGIC_ENABLE = yes      # Virtual DIP switch configuration
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = yes        # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend
NKRO_ENABLE = no            # USB Nkey Rollover
UNICODE_ENABLE = no         # Unicode
ENCODER_ENABLE = yes        # Encoders

CUSTOM_MATRIX = lite
EEPROM_DRIVER = spi

OPT_DEFS += -DCORTEX_ENABLE_WFI_IDLE=TRUE

RGBLIGHT_ENABLE = yes
RGBLIGHT_DRIVER = WS2812

QUANTUM_PAINTER_ENABLE = yes
QUANTUM_PAINTER_DRIVERS = ssd1351_spi

SRC += analog.c