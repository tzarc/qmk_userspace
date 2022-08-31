// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#include <string.h>
#include <quantum.h>
#include "tzarc.h"

struct diablo3_config_t diablo3_config;

static struct d3_desc_ {
    deferred_token token;
    bool           pressed;
} d3_desc[4];

uint32_t            d3_last_exec;
deferred_executor_t d3_execs[4];

void tzarc_diablo3_init(void) {
    memset(&diablo3_config, 0, sizeof(diablo3_config));
}

bool diablo3_key_enabled_get(uint16_t keycode) {
    return BITMASK_BIT_GET(diablo3_config.keys_activated, keycode, KC_1);
}

void diablo3_key_enabled_set(uint16_t keycode, bool on) {
    BITMASK_BIT_ASSIGN(diablo3_config.keys_activated, on, keycode, KC_1);
    tzarc_eeprom_save();
}

bool process_record_diablo3(uint16_t keycode, keyrecord_t *record) {
    // If we're in config mode....
    if (config_enabled) {
        // ...and we're releasing keys 1...4...
        if (KC_1 <= keycode && keycode <= KC_4 && !record->event.pressed) {
            // ...toggle the enabled flag for this key.
            bool new_state = !diablo3_key_enabled_get(keycode);
            diablo3_key_enabled_set(keycode, new_state);
            dprintf("[D3] Key repeat on %s: %s\n", key_name(keycode, false), new_state ? "on" : "off");
        }
        // No-op, under normal circumstances.
        return false;
    } else {
        if (keycode == KC_ESCAPE || keycode == KC_GESC) {
            // If any executors are active, capture <ESC> and prevent it from functioning.
            // On release of <ESC>, disable all executors. Effectively skips the first <ESC>.
            bool any_active = false;
            for (uint16_t kc = KC_1; kc <= KC_4; ++kc) {
                if (diablo3_key_enabled_get(kc)) {
                    any_active = true;
                    if (!record->event.pressed) {
                        diablo3_key_enabled_set(kc, false);
                        dprintf("[D3] Key repeat on %s: %s\n", key_name(kc, false), "off");
                    }
                }
            }
            if (any_active) {
                return false;
            }
        } else {
            if (diablo3_key_enabled_get(keycode)) {
                // If we're enabled...... don't bother doing anything here.
                // Automatic keypresses are implemented in matrix_scan_diablo3.
                return false;
            }
        }
    }

    // If we get here, fallback to normal key functionality.
    return process_record_keymap(keycode, record);
}

uint32_t diablo3_deferred_exec_callback(uint32_t trigger_time, void *cb_arg) {
    // Work out which key we care about
    uint8_t idx = (uint8_t)(uintptr_t)cb_arg;

    // Figure out if we're doing keyup or keydown
    if (!d3_desc[idx].pressed) {
        register_code16(idx + KC_1);
    } else {
        unregister_code16(idx + KC_1);
    }

    // Toggle it
    d3_desc[idx].pressed = !d3_desc[idx].pressed;

    // Signal the next repeat time
    return (tzarc_eeprom_cfg.d3_delays[idx] * 1000) + 128 + prng() % 128;
}

void disable_automatic_diablo3_key(uint16_t kc) {
    uint8_t idx = kc - KC_1;
    // If this key is disabled, kill any deferred executor if it's present.
    if (d3_desc[idx].token != INVALID_DEFERRED_TOKEN) {
        cancel_deferred_exec_advanced(d3_execs, 4, d3_desc[idx].token);
        d3_desc[idx].token = INVALID_DEFERRED_TOKEN;
    }

    // Release the key if it was pressed.
    if (d3_desc[idx].pressed) {
        unregister_code16(kc);
        d3_desc[idx].pressed = false;
    }

    if (diablo3_key_enabled_get(kc)) {
        diablo3_key_enabled_set(kc, false);
        dprintf("[D3] Key repeat on %s: %s\n", key_name(kc, false), "off");
    }
}

void disable_automatic_diablo3(void) {
    for (uint16_t kc = KC_1; kc <= KC_4; ++kc) {
        disable_automatic_diablo3_key(kc);
    }
}

void matrix_scan_diablo3(void) {
    for (uint16_t kc = KC_1; kc <= KC_4; ++kc) {
        if (diablo3_key_enabled_get(kc)) {
            // If this key is enabled, but we don't yet have a deferred executor running... start one.
            uint8_t idx = kc - KC_1;
            if (d3_desc[idx].token == INVALID_DEFERRED_TOKEN) {
                d3_desc[idx].token = defer_exec_advanced(d3_execs, 4,
                                                         (tzarc_eeprom_cfg.d3_delays[idx] * 1000) + 128 + prng() % 128, // Eventually configurable
                                                         diablo3_deferred_exec_callback, (void *)(uintptr_t)idx);
            }
        } else {
            disable_automatic_diablo3_key(kc);
        }
    }

    // Run any deferred executors if they're still active.
    deferred_exec_advanced_task(d3_execs, 4, &d3_last_exec);
}
