## chip/board settings
# the next two should match the directories in
#  <chibios>/os/hal/ports/$(MCU_FAMILY)/$(MCU_SERIES)
MCU_FAMILY = STM32
MCU_SERIES = STM32L0xx

# linker script to use
MCU_LDSCRIPT = STM32L082xZ

# startup code to use
# is should exist in <chibios>/os/common/ports/ARMCMx/compilers/GCC/mk/
MCU_STARTUP = stm32l0xx

# it should exist either in <chibios>/os/hal/boards/
#  or <this_dir>/boards
BOARD = ST_NUCLEO32_L082KZ

# Cortex version
MCU  = cortex-m0plus

# ARM version, CORTEX-M0/M1 are 6, CORTEX-M3/M4/M7 are 7
ARMV = 6

# Build Options
#BOOTMAGIC_ENABLE = yes    # Virtual DIP switch configuration
#MOUSEKEY_ENABLE = yes     # Mouse keys
#EXTRAKEY_ENABLE = yes     # Audio control and System control
CONSOLE_ENABLE = yes      # Console for debug
# COMMAND_ENABLE = yes      # Commands for debug and configuration
#NKRO_ENABLE = yes         # USB Nkey Rollover
CUSTOM_MATRIX = yes       # Custom matrix file
#RAW_ENABLE = yes
# SLEEP_LED_ENABLE = yes    # Breathing sleep LED during USB suspend

# RGBLIGHT_ENABLE = yes
RGB_MATRIX_ENABLE = WS2812
WS2812_DRIVER = bitbang

#EEPROM_DRIVER = i2c

#OLED_DRIVER_ENABLE = yes
# AUDIO_ENABLE = yes
# LINK_TIME_OPTIMIZATION_ENABLE = yes

SRC += \
	bootloader.c \
	shiftreg_matrix.c

DFU_ARGS = -d 0483:df11 -a 0 -s 0x08000000:leave
