// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H

enum layer_names {
    _QWERTY,
    _DPAD,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
   [_QWERTY] = LAYOUT_split_3x5_3(
     KC_Q,    KC_W,    KC_E,      KC_R,       KC_T,    KC_Y,    KC_U,    KC_I,      KC_O,     KC_P,
     KC_A,    KC_S,    KC_D,      KC_F,       KC_G,    KC_H,    KC_J,    KC_K,      KC_L,     KC_SCLN,
     KC_Z,    KC_X,    KC_C,      KC_V,       KC_B,    KC_N,    KC_M,    KC_COMM,   KC_DOT,   KC_SLSH,
                       KC_LSFT,   KC_LALT,    KC_ENT,  KC_SPC,  MO(_DPAD),  KC_RCTL
    ),
    [_DPAD] = LAYOUT_split_3x5_3(
        KC_TRNS, KC_UP, KC_TRNS,   KC_TRNS,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS,
        KC_LEFT, KC_DOWN, KC_RIGHT,   KC_TRNS,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS,   KC_TRNS, KC_TRNS

    )
};
// clang-format on

const key_override_t grave_ctrldel_override = ko_make_basic(MOD_MASK_CTRL, KC_DEL, KC_GRV);

const key_override_t *key_overrides[] = {&grave_ctrldel_override};
