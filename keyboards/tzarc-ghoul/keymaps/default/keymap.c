// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,           RGB_MOD,          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                             KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
};
// clang-format on

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = {ENCODER_CCW_CW(RGB_HUI, RGB_HUD)},
};

#ifdef DEBUG_EEPROM_OUTPUT

#    ifdef WEAR_LEVELING_ENABLE
#        include "wear_leveling.h"
#    endif // WEAR_LEVELING_ENABLE

uint8_t prng(void) {
    static uint8_t s = 0xAA, a = 0;
    s ^= s << 3;
    s ^= s >> 5;
    s ^= a++ >> 2;
    return s;
}

void keyboard_post_init_user(void) {
    // Customise these values to desired behaviour
    debug_enable = true;
    // debug_matrix = true;
    // debug_keyboard=true;
    // debug_mouse=true;
}

void matrix_scan_user(void) {
    static uint32_t last_eeprom_access = 0;
    uint32_t        now                = timer_read32();
    if (now - last_eeprom_access > 5000) {
        dprint("reading eeprom\n");
        last_eeprom_access = now;

        union {
            uint8_t  bytes[4];
            uint32_t raw;
        } tmp;
        extern uint8_t prng(void);
        tmp.bytes[0] = prng();
        tmp.bytes[1] = prng();
        tmp.bytes[2] = prng();
        tmp.bytes[3] = prng();

        eeconfig_update_user(tmp.raw);
        uint32_t value = eeconfig_read_user();
        if (value != tmp.raw) {
            dprint("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            dprint("!! EEPROM readback mismatch!\n");
            dprint("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        }
    }

#    ifdef WEAR_LEVELING_ENABLE
    static uint32_t last_wear_leveling_init = 0;
    if (now - last_wear_leveling_init > 30000) {
        dprint("init'ing wear-leveling\n");
        last_wear_leveling_init = now;
        wear_leveling_init();
    }
#    endif // WEAR_LEVELING_ENABLE
}

#endif // DEBUG_EEPROM_OUTPUT
