// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// 1000Hz poll rate
#define USB_POLLING_INTERVAL_MS 1

// Encoder settings
#define ENCODER_RESOLUTION 2

// LCD blanking period
#define LCD_ACTIVITY_TIMEOUT 30000

// RGB settings
#define RGBLIGHT_ANIMATIONS
#define RGB_MATRIX_KEYPRESSES
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS

// Allow for an extra sync command over the split
#define SPLIT_TRANSACTION_IDS_USER RPC_ID_SYNC_STATE_USER, RPC_ID_GET_COUNTER

//#define QUANTUM_PAINTER_DEBUG