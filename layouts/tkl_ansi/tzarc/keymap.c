// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "tzarc.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT_tkl_ansi_wrapper(
      ____________TZARC_BASE_R1_ALPHA_ESC______, ____________TZARC_BASE_R1_NAV____________,
      ____________TZARC_BASE_R2_ALPHA_GRV______, ____________TZARC_BASE_R2_NAV____________,
      ____________TZARC_BASE_R3_ALPHA__________, ____________TZARC_BASE_R3_NAV____________,
      ____________TZARC_BASE_R4_ALPHA__________,
      ____________TZARC_BASE_R5_ALPHA__________, ____________TZARC_BASE_R5_NAV____________,
      ____________TZARC_BASE_R6_ALPHA__________, ____________TZARC_BASE_R6_NAV____________),

  [LAYER_LOWER] = LAYOUT_tkl_ansi_wrapper(
      ____________TZARC_LOWER_R1_ALPHA_________, ____________TZARC_LOWER_R1_NAV___________,
      ____________TZARC_LOWER_R2_ALPHA_NORM____, ____________TZARC_LOWER_R2_NAV___________,
      ____________TZARC_LOWER_R3_ALPHA_________, ____________TZARC_LOWER_R3_NAV___________,
      ____________TZARC_LOWER_R4_ALPHA_________,
      ____________TZARC_LOWER_R5_ALPHA_________, ____________TZARC_LOWER_R5_NAV___________,
      ____________TZARC_LOWER_R6_ALPHA_________, ____________TZARC_LOWER_R6_NAV___________),

  [LAYER_RAISE] = LAYOUT_tkl_ansi_wrapper(
      ____________TZARC_RAISE_R1_ALPHA_________, ____________TZARC_RAISE_R1_NAV___________,
      ____________TZARC_RAISE_R2_ALPHA_________, ____________TZARC_RAISE_R2_NAV___________,
      ____________TZARC_RAISE_R3_ALPHA_________, ____________TZARC_RAISE_R3_NAV___________,
      ____________TZARC_RAISE_R4_ALPHA_________,
      ____________TZARC_RAISE_R5_ALPHA_________, ____________TZARC_RAISE_R5_NAV___________,
      ____________TZARC_RAISE_R6_ALPHA_________, ____________TZARC_RAISE_R6_NAV___________),

  [LAYER_ADJUST] = LAYOUT_tkl_ansi_wrapper(
      ____________TZARC_ADJUST_R1_ALPHA________, ____________TZARC_ADJUST_R1_NAV__________,
      ____________TZARC_ADJUST_R2_ALPHA________, ____________TZARC_ADJUST_R2_NAV__________,
      ____________TZARC_ADJUST_R3_ALPHA________, ____________TZARC_ADJUST_R3_NAV__________,
      ____________TZARC_ADJUST_R4_ALPHA________,
      ____________TZARC_ADJUST_R5_ALPHA________, ____________TZARC_ADJUST_R5_NAV__________,
      ____________TZARC_ADJUST_R6_ALPHA________, ____________TZARC_ADJUST_R6_NAV__________),
};
// clang-format on
