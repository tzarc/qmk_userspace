// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "quantum.h"

// clang-format off
#define LAYOUT_ortho_2x8(k00, k01, k02, k03, k04, k05, k06, k07,            \
                         k10, k11, k12, k13, k14, k15, k16, k17)            \
    {                                                                       \
        { k00, k01, k02, k03, k04, k05, k06, k07 },                         \
        { k10, k11, k12, k13, k14, k15, k16, k17 }                          \
    }
// clang-format off
