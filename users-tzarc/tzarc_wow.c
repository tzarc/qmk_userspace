// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <string.h>
#include <quantum.h>
#include "tzarc.h"

#ifndef __AVR__
static const char* const wow_keycode_display_map[][2] = {
    [WOW_KEY_OFFSET(KC_A)]      = {"a", "A"},
    [WOW_KEY_OFFSET(KC_B)]      = {"b", "B"},
    [WOW_KEY_OFFSET(KC_C)]      = {"c", "C"},
    [WOW_KEY_OFFSET(KC_D)]      = {"d", "D"},
    [WOW_KEY_OFFSET(KC_E)]      = {"e", "E"},
    [WOW_KEY_OFFSET(KC_F)]      = {"f", "F"},
    [WOW_KEY_OFFSET(KC_G)]      = {"g", "G"},
    [WOW_KEY_OFFSET(KC_H)]      = {"h", "H"},
    [WOW_KEY_OFFSET(KC_I)]      = {"i", "I"},
    [WOW_KEY_OFFSET(KC_J)]      = {"j", "J"},
    [WOW_KEY_OFFSET(KC_K)]      = {"k", "K"},
    [WOW_KEY_OFFSET(KC_L)]      = {"l", "L"},
    [WOW_KEY_OFFSET(KC_M)]      = {"m", "M"},
    [WOW_KEY_OFFSET(KC_N)]      = {"n", "N"},
    [WOW_KEY_OFFSET(KC_O)]      = {"o", "O"},
    [WOW_KEY_OFFSET(KC_P)]      = {"p", "P"},
    [WOW_KEY_OFFSET(KC_Q)]      = {"q", "Q"},
    [WOW_KEY_OFFSET(KC_R)]      = {"r", "R"},
    [WOW_KEY_OFFSET(KC_S)]      = {"s", "S"},
    [WOW_KEY_OFFSET(KC_T)]      = {"t", "T"},
    [WOW_KEY_OFFSET(KC_U)]      = {"u", "U"},
    [WOW_KEY_OFFSET(KC_V)]      = {"v", "V"},
    [WOW_KEY_OFFSET(KC_W)]      = {"w", "W"},
    [WOW_KEY_OFFSET(KC_X)]      = {"x", "X"},
    [WOW_KEY_OFFSET(KC_Y)]      = {"y", "Y"},
    [WOW_KEY_OFFSET(KC_Z)]      = {"z", "Z"},
    [WOW_KEY_OFFSET(KC_1)]      = {"1", "!"},
    [WOW_KEY_OFFSET(KC_2)]      = {"2", "@"},
    [WOW_KEY_OFFSET(KC_3)]      = {"3", "#"},
    [WOW_KEY_OFFSET(KC_4)]      = {"4", "$"},
    [WOW_KEY_OFFSET(KC_5)]      = {"5", "%"},
    [WOW_KEY_OFFSET(KC_6)]      = {"6", "^"},
    [WOW_KEY_OFFSET(KC_7)]      = {"7", "&"},
    [WOW_KEY_OFFSET(KC_8)]      = {"8", "*"},
    [WOW_KEY_OFFSET(KC_9)]      = {"9", "("},
    [WOW_KEY_OFFSET(KC_0)]      = {"0", ")"},
    [WOW_KEY_OFFSET(KC_ENTER)]  = {"Ent", "Ent"},
    [WOW_KEY_OFFSET(KC_ESCAPE)] = {"Esc", "Esc"},
    [WOW_KEY_OFFSET(KC_BSPACE)] = {"Bksp", "Bksp"},
    [WOW_KEY_OFFSET(KC_TAB)]    = {"Tab", "Tab"},
    [WOW_KEY_OFFSET(KC_SPACE)]  = {"Spc", "Spc"},
    [WOW_KEY_OFFSET(KC_MINUS)]  = {"-", "_"},
    [WOW_KEY_OFFSET(KC_EQUAL)]  = {"=", "="},
#    if defined(QUANTUM_PAINTER)
    [WOW_KEY_OFFSET(KC_LBRACKET)]   = {"[", "{"},
    [WOW_KEY_OFFSET(KC_RBRACKET)]   = {"]", "}"},
    [WOW_KEY_OFFSET(KC_BSLASH)]     = {"\\", "|"},
    [WOW_KEY_OFFSET(KC_NONUS_HASH)] = {"#", "#"},
    [WOW_KEY_OFFSET(KC_SCOLON)]     = {";", ":"},
    [WOW_KEY_OFFSET(KC_QUOTE)]      = {"'", "\""},
    [WOW_KEY_OFFSET(KC_GRAVE)]      = {"`", "~"},
    [WOW_KEY_OFFSET(KC_COMMA)]      = {",", "<"},
    [WOW_KEY_OFFSET(KC_DOT)]        = {".", ">"},
    [WOW_KEY_OFFSET(KC_SLASH)]      = {"/", "?"},
    [WOW_KEY_OFFSET(KC_CAPSLOCK)]   = {"Caps", "Caps"},
    [WOW_KEY_OFFSET(KC_F1)]         = {"F1", "F1"},
    [WOW_KEY_OFFSET(KC_F2)]         = {"F2", "F2"},
    [WOW_KEY_OFFSET(KC_F3)]         = {"F3", "F3"},
    [WOW_KEY_OFFSET(KC_F4)]         = {"F4", "F4"},
    [WOW_KEY_OFFSET(KC_F5)]         = {"F5", "F5"},
    [WOW_KEY_OFFSET(KC_F6)]         = {"F6", "F6"},
    [WOW_KEY_OFFSET(KC_F7)]         = {"F7", "F7"},
    [WOW_KEY_OFFSET(KC_F8)]         = {"F8", "F8"},
    [WOW_KEY_OFFSET(KC_F9)]         = {"F9", "F9"},
    [WOW_KEY_OFFSET(KC_F10)]        = {"F10", "F10"},
    [WOW_KEY_OFFSET(KC_F11)]        = {"F11", "F11"},
    [WOW_KEY_OFFSET(KC_F12)]        = {"F12", "F12"},
    [WOW_KEY_OFFSET(KC_PSCREEN)]    = {"PScr", "PScr"},
    [WOW_KEY_OFFSET(KC_SCROLLLOCK)] = {"ScrL", "ScrL"},
    [WOW_KEY_OFFSET(KC_PAUSE)]      = {"Paus", "Paus"},
    [WOW_KEY_OFFSET(KC_INSERT)]     = {"Ins", "Ins"},
    [WOW_KEY_OFFSET(KC_HOME)]       = {"Home", "Home"},
    [WOW_KEY_OFFSET(KC_PGUP)]       = {"PgUp", "PgUp"},
    [WOW_KEY_OFFSET(KC_DELETE)]     = {"Del", "Del"},
    [WOW_KEY_OFFSET(KC_END)]        = {"End", "End"},
    [WOW_KEY_OFFSET(KC_PGDOWN)]     = {"PgDn", "PgDn"},
    [WOW_KEY_OFFSET(KC_RIGHT)]      = {"Rght", "Rght"},
    [WOW_KEY_OFFSET(KC_LEFT)]       = {"Left", "Left"},
    [WOW_KEY_OFFSET(KC_DOWN)]       = {"Up", "Up"},
    [WOW_KEY_OFFSET(KC_UP)]         = {"Down", "Down"},
    [WOW_KEY_OFFSET(KC_NUMLOCK)]    = {"NumL", "NumL"},
#    endif // defined(QUANTUM_PAINTER)
};

const char* wow_key_char(uint16_t keycode) {
    return wow_keycode_display_map[WOW_KEY_OFFSET(keycode)][0];
}
#else
const char* wow_key_char(uint16_t keycode) {
    return "Unknown";
}
#endif // __AVR__

struct wow_config_t wow_config;

bool wow_key_enabled_get(uint16_t keycode) {
    return BITMASK_BIT_GET(tzarc_eeprom_cfg.wow_enabled, keycode, WOW_KEY_MIN);
}
void wow_key_enabled_set(uint16_t keycode, bool on) {
    BITMASK_BIT_ASSIGN(tzarc_eeprom_cfg.wow_enabled, on, keycode, WOW_KEY_MIN);
    tzarc_eeprom_save();
}

bool wow_key_keydown_get(uint16_t keycode) {
    return BITMASK_BIT_GET(wow_config.keydown, keycode, WOW_KEY_MIN);
}
void wow_key_keydown_set(uint16_t keycode, bool on) {
    BITMASK_BIT_ASSIGN(wow_config.keydown, on, keycode, WOW_KEY_MIN);
}

bool wow_key_released_get(uint16_t keycode) {
    return BITMASK_BIT_GET(wow_config.released, keycode, WOW_KEY_MIN);
}
void wow_key_released_set(uint16_t keycode, bool on) {
    BITMASK_BIT_ASSIGN(wow_config.released, on, keycode, WOW_KEY_MIN);
}

bool wow_key_auto_registered_get(uint16_t keycode) {
    return BITMASK_BIT_GET(wow_config.auto_registered, keycode, WOW_KEY_MIN);
}
void wow_key_auto_registered_set(uint16_t keycode, bool on) {
    BITMASK_BIT_ASSIGN(wow_config.auto_registered, on, keycode, WOW_KEY_MIN);
}

uint32_t wow_key_last_keydown_get(uint16_t keycode) {
    return wow_config.last_keydown[WOW_KEY_OFFSET(keycode)];
}
void wow_key_last_keydown_set(uint16_t keycode, uint32_t last_keydown) {
    wow_config.last_keydown[WOW_KEY_OFFSET(keycode)] = last_keydown;
}

uint32_t wow_key_next_trigger_get(uint16_t keycode) {
    return wow_config.next_trigger[WOW_KEY_OFFSET(keycode)];
}
void wow_key_next_trigger_set(uint16_t keycode, uint32_t next_trigger) {
    wow_config.next_trigger[WOW_KEY_OFFSET(keycode)] = next_trigger;
}

void tzarc_wow_init(void) {
    memset(&wow_config, 0, sizeof(wow_config));
}

bool process_record_wow(uint16_t keycode, keyrecord_t* record) {
    if (config_enabled) {
        if (!record->event.pressed) {
            // Toggle the enabled flag for this key
            bool new_state = !wow_key_enabled_get(keycode);
            wow_key_enabled_set(keycode, new_state);
            dprintf("Key repeat on %s: %s\n", wow_key_char(keycode), new_state ? "on" : "off");
        }
        return false;
    } else {
        // Fallback to the normal key if not enabled
        if (!wow_key_enabled_get(keycode)) {
            return process_record_keymap(keycode, record);
        }

        uint32_t now = timer_read32();
        if (record->event.pressed) {
            // Keydown event
            wow_key_keydown_set(keycode, true);
            wow_key_released_set(keycode, false);

            // Keep track of last keydown, as well as next trigger time
            wow_key_last_keydown_set(keycode, now);
            wow_key_next_trigger_set(keycode, now + 128 + (prng() % 128));

            // Inform the OS that we've got a keydown event
            register_code(keycode);
        } else {
            // Keyup event
            wow_key_keydown_set(keycode, false);

            // If the release happened within the initial hold period, then stop the timer and tap the key as per normal
            if ((now < wow_key_next_trigger_get(keycode) && !wow_key_released_get(keycode)) || wow_key_auto_registered_get(keycode)) {
                unregister_code(keycode);
                wow_key_auto_registered_set(keycode, false);
            }
        }
        return false;
    }

    return process_record_keymap(keycode, record);
}

void matrix_scan_wow(void) {
    uint32_t now = timer_read32();

    // Loop through A-Z, 0-9, Enter, Esc, Backspace, Tab, Space
    for (uint16_t keycode = WOW_KEY_MIN; keycode <= WOW_KEY_MAX; ++keycode) {
        // If this key is enabled for repeat, is held down, and we've passed the trigger point...
        if (wow_key_enabled_get(keycode) && wow_key_keydown_get(keycode) && wow_key_next_trigger_get(keycode) <= now) {
            // Check if we've not yet released due to the initial hold period
            if (!wow_key_released_get(keycode)) {
                // ...if we haven't, then we release the key.
                unregister_code(keycode);
                wow_key_released_set(keycode, true);
                wow_key_auto_registered_set(keycode, false);
            } else {
                // Toggle the keypress with random timing below
                if (!wow_key_auto_registered_get(keycode)) {
                    register_code(keycode);
                    wow_key_auto_registered_set(keycode, true);
                } else {
                    unregister_code(keycode);
                    wow_key_auto_registered_set(keycode, false);
                }
            }

            // Set the next trigger.
            wow_key_next_trigger_set(keycode, now + 64 + (prng() % 64));
        }
    }
}
