// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <quantum.h>

#define ___ KC_NO

// clang-format off
#define LAYOUT_all( \
        k00, k01, k02, k03, k04,   kEC,   k05, k06, k07, k08, k09, \
        k10, k11, k12, k13, k14,          k15, k16, k17, k18, k19, \
        k20, k21, k22, k23, k24,          k25, k26, k27, k28, k29, \
        k30, k31, k32, k33, k34,          k35, k36, k37, k38, k39) \
    {                                                              \
        { k00, k10, k20, k30, k01, k11, k21, k31 },                \
        { k02, k12, k22, k32, k03, k13, k23, k33 },                \
        { k04, k14, k24, k34, k05, k15, k25, k35 },                \
        { k06, k16, k26, k36, k07, k17, k27, k37 },                \
        { k08, k18, k28, k38, k09, k19, k29, k39 },                \
        { kEC, ___, ___, ___, ___, ___, ___, ___ }                 \
    }
// clang-format on
