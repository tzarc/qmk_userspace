// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <ctype.h>
#include <quantum.h>
#include "keycodes.h"
#include "modifiers.h"
#include "quantum_keycodes.h"
#include "tzarc_layout.h"
#include "util.h"

#ifndef __AVR__
static const char *const keycode_display_map[][2] = {
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

static const char *key_name_hid(uint16_t keycode, bool shifted) {
#    ifndef QUANTUM_PAINTER_ENABLE
    if (keycode > KC_EQUAL) {
        return "Unknown";
    }
#    else
    if (keycode > KC_NUM_LOCK) {
        return "Unknown";
    }
#    endif
    return keycode_display_map[keycode][shifted ? 1 : 0];
}
#else  // __AVR__
static const char *key_name_hid(uint16_t keycode, bool shifted) {
    static char buffer[16];
    const char *f = get_numeric_str(buffer, sizeof(buffer), keycode, ' ');
    while (*f == ' ')
        ++f;
    return f;
}
#endif // __AVR__

void fill_one_param_name(char *buffer, const char *name, const char *param1, size_t buffer_length) {
    memset(buffer, 0, buffer_length);
    strlcpy(buffer, name, buffer_length);
    strlcat(buffer, "(", buffer_length);
    strlcat(buffer, param1, buffer_length);
    strlcat(buffer, ")", buffer_length);
}

void fill_two_param_name(char *buffer, const char *name, const char *param1, const char *param2, size_t buffer_length) {
    memset(buffer, 0, buffer_length);
    strlcpy(buffer, name, buffer_length);
    strlcat(buffer, "(", buffer_length);
    strlcat(buffer, param1, buffer_length);
    strlcat(buffer, ",", buffer_length);
    strlcat(buffer, param2, buffer_length);
    strlcat(buffer, ")", buffer_length);
}

static const char *layer_name_pretty(const char *input) {
    static char buffer[16];
    strlcpy(buffer, input, sizeof(buffer));
    if (memcmp(buffer, "LAYER_", 6) == 0) {
        int len   = strlen(buffer);
        buffer[6] = toupper(buffer[6]);
        for (int i = 7; i < len; i++) {
            buffer[i] = tolower(buffer[i]);
        }
        return &buffer[6];
    }
    return buffer;
}

const char *layer_name(uint8_t layer) {
    static char buffer[16];
    switch (layer) {
#define LAYER_SWITCH_CASE(n) \
    case n:                  \
        return layer_name_pretty(#n);

        FOREACH_CUSTOM_LAYER(LAYER_SWITCH_CASE)

        default:
            break;
    }

    const char *n = get_numeric_str(buffer, sizeof(buffer), layer, ' ');
    while (*n == ' ')
        ++n;
    return n;
}

const char *mod_name(uint16_t mod) {
    static char buffer[16];
    static struct {
        uint8_t mod;
        uint8_t index;
    } mods[] = {
        {MOD_LSFT, 1}, {MOD_RSFT, 6}, {MOD_LCTL, 2}, {MOD_RCTL, 7}, {MOD_LALT, 3}, {MOD_RALT, 8}, {MOD_LGUI, 4}, {MOD_RGUI, 9},
    };
    strlcpy(buffer, "LscagRscag", sizeof(buffer));
    for (int i = 0; i < ARRAY_SIZE(mods); ++i) {
        if ((mod & mods[i].mod) == mods[i].mod) {
            buffer[mods[i].index] = toupper(buffer[mods[i].index]);
        }
    }
    return buffer;
}

const char *key_name(uint16_t keycode, bool shifted) {
    static char buffer[32];
    char        buf1[16];
    char        buf2[16];
    (void)buffer;
    (void)buf1;
    (void)buf2;
    switch (keycode) {
        case KC_A ... KC_NUM_LOCK: {
            const char *name = key_name_hid(keycode, shifted);
            if (!name) return "Unknown";
            return name;
        }
        case KC_LEFT_GUI:
            return "L.GUI";
        case KC_RIGHT_GUI:
            return "R.GUI";
        case KC_LEFT_CTRL:
            return "L.Ctl";
        case KC_RIGHT_CTRL:
            return "R.Ctl";
        case KC_LEFT_ALT:
            return "L.Alt";
        case KC_RIGHT_ALT:
            return "R.Alt";
        case KC_LEFT_SHIFT:
            return "L.Sft";
        case KC_RIGHT_SHIFT:
            return "R.Sft";
        /*
        case QK_MOMENTARY ... QK_MOMENTARY_MAX: {
            const char *n = get_numeric_str(buf1, sizeof(buf1), QK_MOMENTARY_GET_LAYER(keycode), ' ');
            while (*n == ' ')
                ++n;
            fill_one_param_name(buffer, "MO", n, sizeof(buffer));
            return buffer;
        }
        case QK_MOD_TAP ... QK_MOD_TAP_MAX: {
            fill_two_param_name(buffer, "MT", mod_name(QK_MOD_TAP_GET_MODS(keycode)), key_name_hid(QK_MOD_TAP_GET_TAP_KEYCODE(keycode), shifted), sizeof(buffer));
            return buffer;
        }
        case QK_LAYER_TAP ... QK_LAYER_TAP_MAX: {
            fill_two_param_name(buffer, "LT", layer_name(QK_LAYER_TAP_GET_LAYER(keycode)), key_name_hid(QK_LAYER_TAP_GET_TAP_KEYCODE(keycode), shifted), sizeof(buffer));
            return buffer;
        }
        */
        case KC_LOWER:
            return "Lower";
        case KC_RAISE:
            return "Raise";
        case QK_GESC:
            return "G.Esc";
        case CHANGEMODE:
            return "Cfg";
    }
    return "Unknown";
}
