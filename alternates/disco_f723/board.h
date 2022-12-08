// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#define STM32_HSECLK                25000000U

#include_next <board.h>

#undef STM32F722xx
#define STM32F723xx

#define BOARD_OTG2_USES_ULPI
