# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

# Cortex version
MCU = cortex-m3

# ARM version, CORTEX-M0/M1 are 6, CORTEX-M3/M4/M7 are 7
ARMV = 7

## chip/board settings
# - the next two should match the directories in
#   <chibios>/os/hal/ports/$(MCU_FAMILY)/$(MCU_SERIES)
MCU_FAMILY = STM32
MCU_SERIES = STM32L1xx

# Linker script to use
# - it should exist either in <chibios>/os/common/ports/ARMCMx/compilers/GCC/ld/
#   or <keyboard_dir>/ld/
MCU_LDSCRIPT ?= STM32L152xE

# Startup code to use
#  - it should exist in <chibios>/os/common/startup/ARMCMx/compilers/GCC/mk/
MCU_STARTUP ?= stm32l1xx

# Board: it should exist either in <chibios>/os/hal/boards/,
# <keyboard_dir>/boards/, or drivers/boards/
BOARD ?= ST_NUCLEO64_L152RE

USE_FPU ?= no

# Options to pass to dfu-util when flashing
DFU_ARGS ?= -d 0483:df11 -a 0 -s 0x08000000:leave
DFU_SUFFIX_ARGS ?= -v 0483 -p df11
