// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include_next <board.h>

#undef STM32_HSECLK
#define STM32_HSECLK 25000000

#undef STM32_HSE_BYPASS
