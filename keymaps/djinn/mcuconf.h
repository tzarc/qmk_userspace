// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include_next <mcuconf.h>

#undef STM32_TRNG_USE_RNG1
#define STM32_TRNG_USE_RNG1 TRUE
