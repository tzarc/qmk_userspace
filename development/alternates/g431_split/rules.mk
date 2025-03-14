# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

# Cortex version
MCU = cortex-m4

# ARM version, CORTEX-M0/M1 are 6, CORTEX-M3/M4/M7 are 7
ARMV = 7

## chip/board settings
# - the next two should match the directories in
#   <chibios>/os/hal/ports/$(MCU_FAMILY)/$(MCU_SERIES)
MCU_FAMILY = STM32
MCU_SERIES = STM32G4xx

# Linker script to use
# - it should exist either in <chibios>/os/common/ports/ARMCMx/compilers/GCC/ld/
#   or <keyboard_dir>/ld/
MCU_LDSCRIPT = STM32G431xB

# Startup code to use
#  - it should exist in <chibios>/os/common/startup/ARMCMx/compilers/GCC/mk/
MCU_STARTUP = stm32g4xx

# Board: it should exist either in <chibios>/os/hal/boards/,
# <keyboard_dir>/boards/, or drivers/boards/
BOARD = ST_NUCLEO64_G431RB

USE_FPU = YES

# Options to pass to dfu-util when flashing
DFU_ARGS = -d 0483:df11 -a 0 -s 0x08000000:leave
DFU_SUFFIX_ARGS = -v 0483 -p df11

# Address of the booloader in system memory
STM32_BOOTLOADER_ADDRESS = 0x1FFF0000

CONSOLE_ENABLE = yes

SERIAL_DRIVER = usart

RGBLIGHT_ENABLE = no
#WS2812_DRIVER = pwm

BACKLIGHT_ENABLE = no
#$BACKLIGHT_DRIVER = pwm

AUDIO_ENABLE = no
#AUDIO_DRIVER = pwm_hardware

# Debugging
#OPT_DEFS += -g -O0 -fno-lto
