# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later

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
BOARD = ST_NUCLEO64_L073RZ

# Cortex version
MCU  = cortex-m0plus

# ARM version, CORTEX-M0/M1 are 6, CORTEX-M3/M4/M7 are 7
ARMV = 6

BOOTLOADER = stm32-dfu

# Build Options
BOOTMAGIC_ENABLE = yes
MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes
CONSOLE_ENABLE = yes
# COMMAND_ENABLE = yes
NKRO_ENABLE = yes
CUSTOM_MATRIX = lite
RAW_ENABLE = no
# SLEEP_LED_ENABLE = yes

RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = WS2812
WS2812_DRIVER = pwm
CIE1931_CURVE = yes

OLED_DRIVER_ENABLE = yes

LTO_ENABLE = yes

SRC += \
	shiftreg_matrix.c

DFU_ARGS = -d 0483:df11 -a 0 -s 0x08000000:leave

# Debugging
#OPT_DEFS += -g -O0 -fno-lto

DEBUG_MATRIX_SCAN_RATE_ENABLE = yes

STM32_BOOTLOADER_ADDRESS = 0x1FF00000

#QUANTUM_PAINTER_ENABLE = yes
#QUANTUM_PAINTER_DRIVERS = qmk_oled_wrapper

# rgb_matrix parser doesn't like this board
ALLOW_WARNINGS = yes
