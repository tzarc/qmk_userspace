// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "tzarc.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT_60_ansi_wrapper(
    ____________TZARC_BASE_R2_ALPHA_GESC_____,
    ____________TZARC_BASE_R3_ALPHA__________,
    ____________TZARC_BASE_R4_ALPHA__________,
    ____________TZARC_BASE_R5_ALPHA__________,
    ____________TZARC_BASE_R6_ALPHA__________),

  [LAYER_LOWER] = LAYOUT_60_ansi_wrapper(
      ____________TZARC_LOWER_R2_ALPHA_FKEYS___,
      ____________TZARC_LOWER_R3_ALPHA_________,
      ____________TZARC_LOWER_R4_ALPHA_________,
      ____________TZARC_LOWER_R5_ALPHA_________,
      ____________TZARC_LOWER_R6_ALPHA_________),

  [LAYER_RAISE] = LAYOUT_60_ansi_wrapper(
      ____________TZARC_RAISE_R2_ALPHA_________,
      ____________TZARC_RAISE_R3_ALPHA_________,
      ____________TZARC_RAISE_R4_ALPHA_________,
      ____________TZARC_RAISE_R5_ALPHA_________,
      ____________TZARC_RAISE_R6_ALPHA_________),

  [LAYER_ADJUST] = LAYOUT_60_ansi_wrapper(
      ____________TZARC_ADJUST_R2_ALPHA________,
      ____________TZARC_ADJUST_R3_ALPHA________,
      ____________TZARC_ADJUST_R4_ALPHA________,
      ____________TZARC_ADJUST_R5_ALPHA________,
      ____________TZARC_ADJUST_R6_ALPHA________),
};
// clang-format on
