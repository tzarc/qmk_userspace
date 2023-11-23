# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

MCU = cortex-m3
ARMV = 7
MCU_FAMILY = STM32
MCU_SERIES = STM32L1xx
MCU_LDSCRIPT ?= STM32L151x8A
MCU_STARTUP ?= stm32l1xx
BOARD ?= ST_NUCLEO64_L152RE
USE_FPU ?= no
DFU_ARGS ?= -d 0483:df11 -a 0 -s 0x08000000:leave
DFU_SUFFIX_ARGS ?= -v 0483 -p df11
