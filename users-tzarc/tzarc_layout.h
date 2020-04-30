/* Copyright 2018-2020 Nick Brassel (@tzarc)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fixes

// Handle expansion of the helpers below
#define LAYOUT_tkl_ansi_wrapper(...) LAYOUT_tkl_ansi(__VA_ARGS__)
#define LAYOUT_60_ansi_wrapper(...) LAYOUT_60_ansi(__VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Layout Helpers: Common

#define ____________TZARC_FKEYS__________________ KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12

#define ____________TZARC_NUMS_1_5_______________ KC_1, KC_2, KC_3, KC_4, KC_5
#define ____________TZARC_NUMS_6_0_______________ KC_6, KC_7, KC_8, KC_9, KC_0
#define ____________TZARC_NUMS_1_0_______________ ____________TZARC_NUMS_1_5_______________, ____________TZARC_NUMS_6_0_______________

#define ____________TZARC_ALPHA_Q_T______________ KC_Q, KC_W, KC_E, KC_R, KC_T
#define ____________TZARC_ALPHA_Y_P______________ KC_Y, KC_U, KC_I, KC_O, KC_P
#define ____________TZARC_ALPHA_Q_P______________ ____________TZARC_ALPHA_Q_T______________, ____________TZARC_ALPHA_Y_P______________

#define ____________TZARC_ALPHA_A_G______________ KC_A, KC_S, KC_D, KC_F, KC_G
#define ____________TZARC_ALPHA_H_SEMICOLON______ KC_H, KC_J, KC_K, KC_L, KC_SCLN
#define ____________TZARC_ALPHA_A_SEMICOLON______ ____________TZARC_ALPHA_A_G______________, ____________TZARC_ALPHA_H_SEMICOLON______

#define ____________TZARC_ALPHA_Z_B______________ KC_Z, KC_X, KC_C, KC_V, KC_B
#define ____________TZARC_ALPHA_N_SLASH__________ KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH
#define ____________TZARC_ALPHA_Z_SLASH__________ ____________TZARC_ALPHA_Z_B______________, ____________TZARC_ALPHA_N_SLASH__________

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Layout Helpers: Standard layout

// LOWER layer
#define ____________TZARC_BASE_R1_ALPHA_ESC______ KC_ESC, ____________TZARC_FKEYS__________________
#define ____________TZARC_BASE_R1_NAV____________ KC_PSCR, KC_SLCK, KC_PAUS

#define ____________TZARC_BASE_R2_ALPHA_GRV______ KC_GRV, ____________TZARC_NUMS_1_0_______________, KC_MINS, KC_EQL, KC_BSPC
#define ____________TZARC_BASE_R2_ALPHA_GESC_____ KC_GESC, ____________TZARC_NUMS_1_0_______________, KC_MINS, KC_EQL, KC_BSPC
#define ____________TZARC_BASE_R2_NAV____________ KC_INS, KC_HOME, KC_PGUP

#define ____________TZARC_BASE_R3_ALPHA__________ KC_TAB_LOWER, ____________TZARC_ALPHA_Q_P______________, KC_LBRC, KC_RBRC, KC_BSLS
#define ____________TZARC_BASE_R3_NAV____________ KC_DEL, KC_END, KC_PGDN

#define ____________TZARC_BASE_R4_ALPHA__________ KC_CTL_ESC, ____________TZARC_ALPHA_A_SEMICOLON______, KC_QUOT, KC_ENT

#define ____________TZARC_BASE_R5_ALPHA__________ KC_LSFT, ____________TZARC_ALPHA_Z_SLASH__________, KC_RSFT
#define ____________TZARC_BASE_R5_NAV____________ KC_UP

#define ____________TZARC_BASE_R6_ALPHA__________ KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, KC_RALT, KC_RGUI, KC_APP_RAISE, KC_RCTL
#define ____________TZARC_BASE_R6_NAV____________ KC_LEFT, KC_DOWN, KC_RGHT

// Lower layer
#define ____________TZARC_LOWER_R1_ALPHA_________ _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
#define ____________TZARC_LOWER_R1_NAV___________ _______, _______, _______

#define ____________TZARC_LOWER_R2_ALPHA_NORM____ _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
#define ____________TZARC_LOWER_R2_ALPHA_FKEYS___ _______, ____________TZARC_FKEYS__________________, _______
#define ____________TZARC_LOWER_R2_NAV___________ _______, _______, _______

#define ____________TZARC_LOWER_R3_ALPHA_________ _______, KC_NOMODE, KC_CONFIG, KC_WOWMODE, KC_D3MODE, _______, _______, _______, _______, _______, _______, KC_VOLD, KC_VOLU, KC_MUTE
#define ____________TZARC_LOWER_R3_NAV___________ _______, _______, _______

#define ____________TZARC_LOWER_R4_ALPHA_________ KC_CAPS, KC_WIDE, KC_SCRIPT, KC_BLOCKS, KC_REGIONAL, KC_AUSSIE, _______, _______, _______, _______, _______, _______, _______

#define ____________TZARC_LOWER_R5_ALPHA_________ _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
#define ____________TZARC_LOWER_R5_NAV___________ _______

#define ____________TZARC_LOWER_R6_ALPHA_________ _______, _______, _______, _______, _______, _______, _______, _______
#define ____________TZARC_LOWER_R6_NAV___________ _______, _______, _______

// Raise layer
#define ____________TZARC_RAISE_R1_ALPHA_________ _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
#define ____________TZARC_RAISE_R1_NAV___________ _______, _______, _______

#define ____________TZARC_RAISE_R2_ALPHA_________ _______, ____________TZARC_FKEYS__________________, _______
#define ____________TZARC_RAISE_R2_NAV___________ _______, _______, _______

#define ____________TZARC_RAISE_R3_ALPHA_________ _______, _______, _______, _______, _______, _______, _______, KC_P7, KC_P8, KC_P9, _______, KC_PSLS, KC_PAST, _______
#define ____________TZARC_RAISE_R3_NAV___________ _______, _______, _______

#define ____________TZARC_RAISE_R4_ALPHA_________ KC_CAPS, _______, _______, _______, _______, _______, _______, KC_P4, KC_P5, KC_P6, _______, _______, _______

#define ____________TZARC_RAISE_R5_ALPHA_________ _______, _______, _______, _______, _______, _______, KC_P0, KC_P1, KC_P2, KC_P3, _______, _______
#define ____________TZARC_RAISE_R5_NAV___________ _______

#define ____________TZARC_RAISE_R6_ALPHA_________ _______, _______, _______, _______, _______, _______, _______, _______
#define ____________TZARC_RAISE_R6_NAV___________ _______, _______, _______

// Adjust layer
#define ____________TZARC_ADJUST_R1_ALPHA________ _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
#define ____________TZARC_ADJUST_R1_NAV__________ _______, _______, _______

#define ____________TZARC_ADJUST_R2_ALPHA________ _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, DEBUG, TIME_EEPRST, TIME_RESET
#define ____________TZARC_ADJUST_R2_NAV__________ _______, _______, _______

#define ____________TZARC_ADJUST_R3_ALPHA________ _______, RGB_MOD, RGB_HUI, RGB_SAI, RGB_VAI, RGB_SPI, _______, _______, _______, _______, _______, _______, _______, _______
#define ____________TZARC_ADJUST_R3_NAV__________ _______, _______, _______

#define ____________TZARC_ADJUST_R4_ALPHA________ KC_CAPS, RGB_RMOD, RGB_HUD, RGB_SAD, RGB_VAD, RGB_SPD, _______, _______, _______, _______, _______, _______, _______

#define ____________TZARC_ADJUST_R5_ALPHA________ _______, BL_DEC, BL_INC, _______, _______, _______, _______, _______, _______, _______, _______, _______
#define ____________TZARC_ADJUST_R5_NAV__________ _______

#define ____________TZARC_ADJUST_R6_ALPHA________ _______, _______, _______, _______, _______, _______, _______, _______
#define ____________TZARC_ADJUST_R6_NAV__________ _______, _______, _______
