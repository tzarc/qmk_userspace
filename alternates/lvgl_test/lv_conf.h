// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// #define LV_DITHER_GRADIENT 1

#include_next <lv_conf.h>

#undef LV_USE_KEYBOARD
#define LV_USE_KEYBOARD 0

#undef LV_USE_MSGBOX
#define LV_USE_MSGBOX 0

#undef LV_USE_TABVIEW
#define LV_USE_TABVIEW 0

#undef LV_USE_TILEVIEW
#define LV_USE_TILEVIEW 0

#undef LV_USE_WIN
#define LV_USE_WIN 0
