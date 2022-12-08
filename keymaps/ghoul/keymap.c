// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#include QMK_KEYBOARD_H
#include "tzarc.h"

enum { _QWERTY, _LOWER, _RAISE, _ADJUST };

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)

// Left-hand home row mods
#define HOME_A LGUI_T(KC_A)
#define HOME_S LALT_T(KC_S)
#define HOME_D LSFT_T(KC_D)
#define HOME_F LCTL_T(KC_F)

// Right-hand home row mods
#define HOME_J RCTL_T(KC_J)
#define HOME_K RSFT_T(KC_K)
#define HOME_L LALT_T(KC_L)
#define HOME_SCLN RGUI_T(KC_SCLN)

#define SFT_ESC LSFT_T(KC_ESC)

extern void ui_init(void);
extern void ui_task(void);

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] = LAYOUT(
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,            RGB_MOD,            KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        HOME_A,  HOME_S,  HOME_D,  HOME_F,  KC_G,                                KC_H,    HOME_J,  HOME_K,  HOME_L,  HOME_SCLN,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                                KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
        SFT_ESC, KC_LCTL, KC_LGUI, KC_SPC,  LOWER,                               RAISE,   KC_SPC,  KC_LALT, KC_BSPC, KC_SFTENT
    ),
    [_LOWER] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,         RGB_MOD,            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [_RAISE] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,         RGB_MOD,            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [_ADJUST] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,         RGB_MOD,            KC_TRNS, KC_TRNS, DB_TOGG, TIME_EEPRST, TIME_RESET,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS
    )
};
// clang-format on

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [_QWERTY] = {ENCODER_CCW_CW(RGB_HUI, RGB_HUD)},
    [_LOWER]  = {ENCODER_CCW_CW(RGB_HUI, RGB_HUD)},
    [_RAISE]  = {ENCODER_CCW_CW(RGB_HUI, RGB_HUD)},
    [_ADJUST] = {ENCODER_CCW_CW(RGB_HUI, RGB_HUD)},
};

void keyboard_post_init_keymap(void) {
    // debug_enable   = true;
    // debug_matrix   = true;
    // debug_keyboard = true;
    // debug_mouse    = true;

    // Init the display
    ui_init();
}

void housekeeping_task_keymap(void) {
    // Draw the display
    ui_task();
}