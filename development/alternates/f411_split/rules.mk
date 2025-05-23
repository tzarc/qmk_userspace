# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

## chip/board settings
# the next two should match the directories in
#  <chibios>/os/hal/ports/$(MCU_FAMILY)/$(MCU_SERIES)
MCU_FAMILY = STM32
MCU_SERIES = STM32F4xx
# linker script to use
# it should exist either in <chibios>/os/common/ports/ARMCMx/compilers/GCC/ld/
#  or <this_dir>/ld/
MCU_LDSCRIPT = STM32F411xE
# startup code to use
# is should exist in <chibios>/os/common/ports/ARMCMx/compilers/GCC/mk/
MCU_STARTUP = stm32f4xx
# it should exist either in <chibios>/os/hal/boards/
#  or <this_dir>/boards
BOARD = BLACKPILL_STM32_F411
# Cortex version
# Teensy LC is cortex-m0; Teensy 3.x are cortex-m4
MCU  = cortex-m4
# ARM version, CORTEX-M0/M1 are 6, CORTEX-M3/M4/M7 are 7
ARMV = 7
USE_FPU = yes
# Address of the booloader in system memory
STM32_BOOTLOADER_ADDRESS ?= 0x1FFF0000

# Options to pass to dfu-util when flashing
DFU_ARGS = -d 0483:df11 -a 0 -s 0x08000000:leave
DFU_SUFFIX_ARGS = -v 0483 -p df11

# Build Options
#   change yes to no to disable
#
KEYBOARD_SHARED_EP       = yes

CONSOLE_ENABLE = yes

SERIAL_DRIVER = usart

RGBLIGHT_ENABLE = yes
WS2812_DRIVER = pwm
CIE1931_CURVE = yes

BACKLIGHT_ENABLE = no
#$BACKLIGHT_DRIVER = pwm

AUDIO_ENABLE = no
#AUDIO_DRIVER = pwm_hardware

# Debugging
#OPT_DEFS += -g -O0 -fno-lto
