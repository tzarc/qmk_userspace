// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

//#define USE_I2C
#define EE_HANDS

#undef RGBLED_NUM
#define RGBLED_NUM 12
#define RGBLIGHT_HUE_STEP 8
#define RGBLIGHT_SAT_STEP 8
#define RGBLIGHT_VAL_STEP 8

#ifdef RGBLIGHT_ENABLE
#    undef RGBLIGHT_ANIMATIONS
#endif // RGBLIGHT_ENABLE

#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION

// Allow for an extra sync command over the split
#define SPLIT_TRANSPORT_MIRROR
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_TRANSACTION_IDS_USER RPC_ID_SLAVE_COUNTER
