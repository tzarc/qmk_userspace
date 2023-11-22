// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <quantum.h>
#include "util.h"

#ifndef __AVR__
static const char* const keycode_display_map[][2] = {
    [KC_A]         = {"a", "A"},
    [KC_B]         = {"b", "B"},
    [KC_C]         = {"c", "C"},
    [KC_D]         = {"d", "D"},
    [KC_E]         = {"e", "E"},
    [KC_F]         = {"f", "F"},
    [KC_G]         = {"g", "G"},
    [KC_H]         = {"h", "H"},
    [KC_I]         = {"i", "I"},
    [KC_J]         = {"j", "J"},
    [KC_K]         = {"k", "K"},
    [KC_L]         = {"l", "L"},
    [KC_M]         = {"m", "M"},
    [KC_N]         = {"n", "N"},
    [KC_O]         = {"o", "O"},
    [KC_P]         = {"p", "P"},
    [KC_Q]         = {"q", "Q"},
    [KC_R]         = {"r", "R"},
    [KC_S]         = {"s", "S"},
    [KC_T]         = {"t", "T"},
    [KC_U]         = {"u", "U"},
    [KC_V]         = {"v", "V"},
    [KC_W]         = {"w", "W"},
    [KC_X]         = {"x", "X"},
    [KC_Y]         = {"y", "Y"},
    [KC_Z]         = {"z", "Z"},
    [KC_1]         = {"1", "!"},
    [KC_2]         = {"2", "@"},
    [KC_3]         = {"3", "#"},
    [KC_4]         = {"4", "$"},
    [KC_5]         = {"5", "%"},
    [KC_6]         = {"6", "^"},
    [KC_7]         = {"7", "&"},
    [KC_8]         = {"8", "*"},
    [KC_9]         = {"9", "("},
    [KC_0]         = {"0", ")"},
    [KC_ENTER]     = {"Ent", "Ent"},
    [KC_ESCAPE]    = {"Esc", "Esc"},
    [KC_BACKSPACE] = {"Bksp", "Bksp"},
    [KC_TAB]       = {"Tab", "Tab"},
    [KC_SPACE]     = {"Spc", "Spc"},
    [KC_MINUS]     = {"-", "_"},
    [KC_EQUAL]     = {"=", "="},
#    if defined(QUANTUM_PAINTER_ENABLE)
    [KC_LEFT_BRACKET]  = {"[", "{"},
    [KC_RIGHT_BRACKET] = {"]", "}"},
    [KC_BACKSLASH]     = {"\\", "|"},
    [KC_NONUS_HASH]    = {"#", "#"},
    [KC_SEMICOLON]     = {";", ":"},
    [KC_QUOTE]         = {"'", "\""},
    [KC_GRAVE]         = {"`", "~"},
    [KC_COMMA]         = {",", "<"},
    [KC_DOT]           = {".", ">"},
    [KC_SLASH]         = {"/", "?"},
    [KC_CAPS_LOCK]     = {"Caps", "Caps"},
    [KC_F1]            = {"F1", "F1"},
    [KC_F2]            = {"F2", "F2"},
    [KC_F3]            = {"F3", "F3"},
    [KC_F4]            = {"F4", "F4"},
    [KC_F5]            = {"F5", "F5"},
    [KC_F6]            = {"F6", "F6"},
    [KC_F7]            = {"F7", "F7"},
    [KC_F8]            = {"F8", "F8"},
    [KC_F9]            = {"F9", "F9"},
    [KC_F10]           = {"F10", "F10"},
    [KC_F11]           = {"F11", "F11"},
    [KC_F12]           = {"F12", "F12"},
    [KC_PRINT_SCREEN]  = {"PScr", "PScr"},
    [KC_SCROLL_LOCK]   = {"ScrL", "ScrL"},
    [KC_PAUSE]         = {"Paus", "Paus"},
    [KC_INSERT]        = {"Ins", "Ins"},
    [KC_HOME]          = {"Home", "Home"},
    [KC_PGUP]          = {"PgUp", "PgUp"},
    [KC_DELETE]        = {"Del", "Del"},
    [KC_END]           = {"End", "End"},
    [KC_PAGE_DOWN]     = {"PgDn", "PgDn"},
    [KC_RIGHT]         = {"Rght", "Rght"},
    [KC_LEFT]          = {"Left", "Left"},
    [KC_DOWN]          = {"Up", "Up"},
    [KC_UP]            = {"Down", "Down"},
    [KC_NUM_LOCK]      = {"NumL", "NumL"},
#    endif // defined(QUANTUM_PAINTER)
};

const char* key_name(uint16_t keycode, bool shifted) {
#    ifndef QUANTUM_PAINTER_ENABLE
    if (keycode > KC_EQUAL) {
        return "Unk";
    }
#    else
    if (keycode > KC_NUM_LOCK) {
        return "Unk";
    }
#    endif
    return keycode_display_map[keycode][shifted ? 1 : 0];
}
#else
const char* key_name(uint16_t keycode, bool shifted) {
    return "Unknown";
}
#endif // __AVR__
