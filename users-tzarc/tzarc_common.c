// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#include <string.h>
#include <quantum.h>
#include <process_unicode_common.h>
#include "tzarc.h"

bool          config_enabled;
typing_mode_t typing_mode;

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_SFT_ENT:
            return TAPPING_TERM + 120;
        default:
            return TAPPING_TERM;
    }
}

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        default:
            return true;
    }
}

bool get_ignore_mod_tap_interrupt(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        default:
            return true;
    }
}

bool get_tapping_force_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        default:
            return true;
    }
}

const char *typing_mode_name(typing_mode_t mode) {
    switch (mode) {
#define XM(mode, keycode, extra, name) \
    case (mode):                       \
        return name;
        TYPING_MODE_LIST(XM)
#undef XM
    }
    return "unknown";
}

uint8_t prng(void) {
    static uint8_t s = 0xAA, a = 0;
    s ^= s << 3;
    s ^= s >> 5;
    s ^= a++ >> 2;
    return s;
}

__attribute__((weak)) void eeconfig_init_keymap(void) {}

__attribute__((weak)) void keyboard_pre_init_keymap(void) {}
__attribute__((weak)) void keyboard_post_init_keymap(void) {}

__attribute__((weak)) bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    return true;
}

__attribute__((weak)) void matrix_scan_keymap(void) {}

__attribute__((weak)) layer_state_t layer_state_set_keymap(layer_state_t state) {
    return state;
}

__attribute__((weak)) void housekeeping_task_keymap(void) {}

void tzarc_common_init(void) {
    config_enabled = false;
}

void eeconfig_init_user(void) {
#ifdef UNICODE_ENABLE
    set_unicode_input_mode(UC_WINC);
#endif
    tzarc_eeprom_reset();
    eeconfig_init_keymap();
}

int8_t tzarc_sendchar(uint8_t c) {
    // TBD: hook

    // This sends it through to console output by default.
    extern int8_t sendchar(uint8_t c);
    return sendchar(c);
}

void keyboard_pre_init_user(void) {
    keyboard_pre_init_keymap();
    print_set_sendchar(tzarc_sendchar);
}

void keyboard_post_init_user(void) {
    tzarc_common_init();
    tzarc_eeprom_init();
    tzarc_wow_init();
    tzarc_diablo3_init();
    tzarc_eeprom_load();
    keyboard_post_init_keymap();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint32_t reset_key_timer  = 0;
    static uint32_t eeprst_key_timer = 0;

    switch (keycode) {
        case CHANGEMODE:
            // Config enabled while pressed
            config_enabled = record->event.pressed;
            dprintf("Config enabled: %s\n", config_enabled ? "true" : "false");
            return false;

        case TIME_RESET:
            if (record->event.pressed) {
                reset_key_timer = timer_read32();
            } else {
                if (timer_elapsed32(reset_key_timer) >= 500) {
                    reset_keyboard();
                }
            }
            return false;

        case TIME_EEPRST:
            if (record->event.pressed) {
                eeprst_key_timer = timer_read32();
            } else {
                if (timer_elapsed32(eeprst_key_timer) >= 500) {
                    eeconfig_init();
                }
            }
            return false;
    }

    if (config_enabled) {
        switch (keycode) {
#define XM(mode, keycode, extra, name)                  \
    case (keycode):                                     \
        extra {                                         \
            if (record->event.pressed) {                \
                if (typing_mode != (mode)) {            \
                    dprint("Enabling " name " mode\n"); \
                }                                       \
                typing_mode = (mode);                   \
            }                                           \
            return false;                               \
        }
            TYPING_MODE_LIST(XM)
#undef XM
        }
        return false;
    }

#ifdef UNICODE_ENABLE
    // Do unicode glyph replacement if required
    if (!process_record_unicode(keycode, record)) {
        return false;
    }
#endif // UNICODE_ENABLE

    if (typing_mode == MODE_WOW) {
        if ((WOW_KEY_MIN <= keycode) && (keycode <= WOW_KEY_MAX)) {
            if (!process_record_wow(keycode, record)) {
                return false;
            }
        }
    } else if (typing_mode == MODE_D3) {
        if (((KC_1 <= keycode) && (keycode <= KC_4)) || keycode == KC_ESCAPE || keycode == QK_GESC) {
            if (!process_record_diablo3(keycode, record)) {
                return false;
            }
        }
    }

    return process_record_keymap(keycode, record);
}

void matrix_scan_user(void) {
    if (typing_mode == MODE_WOW) {
        matrix_scan_wow();
    } else if (typing_mode == MODE_D3) {
        matrix_scan_diablo3();
    }

    matrix_scan_keymap();
}

layer_state_t layer_state_set_user(layer_state_t state) {
    static layer_state_t last_state = ~(layer_state_t)0;
    // Default handler for lower/raise/adjust
    state = update_tri_layer_state(state, LAYER_LOWER, LAYER_RAISE, LAYER_ADJUST);
    if (last_state != state) {
#ifndef __AVR__
        dprintf("Layer state change: %08lX -> %08lX\n", (uint32_t)last_state, (uint32_t)state);
#endif // __AVR__
        last_state = state;
    }
    return layer_state_set_keymap(state);
}

void housekeeping_task_user(void) {
    housekeeping_task_keymap();
}
