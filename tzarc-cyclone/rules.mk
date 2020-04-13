## chip/board settings
# the next two should match the directories in
#  <chibios>/os/hal/ports/$(MCU_FAMILY)/$(MCU_SERIES)
MCU_FAMILY = STM32
MCU_SERIES = STM32L0xx

# linker script to use
MCU_LDSCRIPT = STM32L072xZ

# startup code to use
# is should exist in <chibios>/os/common/ports/ARMCMx/compilers/GCC/mk/
MCU_STARTUP = stm32l0xx

# it should exist either in <chibios>/os/hal/boards/
#  or <this_dir>/boards
BOARD = GENERIC_STM32_L072KZ

# Cortex version
MCU  = cortex-m0plus

# ARM version, CORTEX-M0/M1 are 6, CORTEX-M3/M4/M7 are 7
ARMV = 6

# Build Options
BOOTMAGIC_ENABLE = lite
# MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes
CONSOLE_ENABLE = yes
#COMMAND_ENABLE = yes
# NKRO_ENABLE = yes
CUSTOM_MATRIX = lite
RAW_ENABLE = no
# SLEEP_LED_ENABLE = yes

RGB_MATRIX_ENABLE = WS2812
WS2812_DRIVER = pwm
CIE1931_CURVE = yes

OLED_DRIVER_ENABLE = yes

LINK_TIME_OPTIMIZATION_ENABLE = yes

SRC += \
	shiftreg_matrix.c

DFU_ARGS = -d 0483:df11 -a 0 -s 0x08000000:leave

# Debugging
#OPT_DEFS += -g -O0 -fno-lto
