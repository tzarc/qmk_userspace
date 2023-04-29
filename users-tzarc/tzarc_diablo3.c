// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdint.h>
#include <string.h>
#include <quantum.h>
#include "deferred_exec.h"
#include "timer.h"
#include "tzarc.h"

struct diablo3_runtime_t diablo3_runtime;

uint32_t            d3_last_exec;
deferred_executor_t d3_execs[4];

void tzarc_diablo3_init(void) {
    disable_automatic_diablo3();
}

bool diablo3_key_enabled_get(uint16_t keycode) {
    return BITMASK_BIT_GET(tzarc_eeprom_cfg.d3_enabled, keycode, KC_1);
}

bool diablo3_key_running(uint16_t keycode) {
    return d3_execs[keycode - KC_1].token != INVALID_DEFERRED_TOKEN;
}

bool diablo3_automatic_running(void) {
    for (uint16_t kc = KC_1; kc <= KC_4; ++kc) {
        if (diablo3_key_running(kc)) {
            return true;
        }
    }
    return false;
}

void diablo3_key_enabled_set(uint16_t keycode, bool on) {
    if (diablo3_key_enabled_get(keycode) != on) {
        BITMASK_BIT_ASSIGN(tzarc_eeprom_cfg.d3_enabled, on, keycode, KC_1);
        tzarc_eeprom_mark_dirty();
    }
    dprintf("[D3] Key repeat on %s: %s\n", key_name(keycode, false), on ? "on" : "off");
}

bool process_record_diablo3(uint16_t keycode, keyrecord_t *record) {
    // If any executors are active, capture <ESC> and prevent it from functioning.
    // On release of <ESC>, disable all executors. Effectively skips the first <ESC>.
    if (keycode == KC_ESCAPE || keycode == QK_GESC) {
        bool any_active = false;
        for (uint16_t kc = KC_1; kc <= KC_4; ++kc) {
            if (diablo3_key_running(kc)) {
                any_active = true;
                break;
            }
        }

        // Consume the <ESC> on both keydown/keyup if any executors are active.
        if (any_active || diablo3_runtime.config_mode) {
            // Cancel any automatic keypress processing
            if (!record->event.pressed) {
                if (diablo3_runtime.config_mode) {
                    dprintf("[D3] Config mode deactivated\n");
                }
                disable_automatic_diablo3();
            }
            return false;
        }
    }

    switch (keycode) {
        case TZ_ENC1L:
            if (diablo3_runtime.config_mode && !record->event.pressed) {
                diablo3_runtime.config_selection = (diablo3_runtime.config_selection + 4 - 1) % 4;
                dprintf("[D3] Config selection: %s\n", key_name(KC_1 + diablo3_runtime.config_selection, false));
            }
            return false;
        case TZ_ENC1R:
            if (diablo3_runtime.config_mode && !record->event.pressed) {
                diablo3_runtime.config_selection = (diablo3_runtime.config_selection + 4 + 1) % 4;
                dprintf("[D3] Config selection: %s\n", key_name(KC_1 + diablo3_runtime.config_selection, false));
            }
            return false;
        case TZ_ENC1P:
            if (record->event.pressed) {
                diablo3_runtime.last_config_press = timer_read32();
            } else {
                uint32_t now = timer_read32();
                if (TIMER_DIFF_32(now, diablo3_runtime.last_config_press) > (DIABLO_3_CONFIG_PRESS_DELTA)) {
                    // Long press of the encoder button -- we want to go into D3 config mode, <ESC> to exit.
                    if (!diablo3_runtime.config_mode) {
                        dprintf("[D3] Config mode activated, current selection: %s\n", key_name(KC_1 + diablo3_runtime.config_selection, false));
                    }
                    disable_automatic_diablo3();
                    diablo3_runtime.config_mode = true;
                } else {
                    // Short press of the encoder button
                    if (diablo3_runtime.config_mode) {
                        // Toggle the current selection
                        diablo3_key_enabled_set(KC_1 + diablo3_runtime.config_selection, !diablo3_key_enabled_get(KC_1 + diablo3_runtime.config_selection));
                    } else {
                        enable_automatic_diablo3();
                    }
                }
                diablo3_runtime.last_config_press = 0;
            }
            return false;

        case KC_1:
        case KC_2:
        case KC_3:
        case KC_4:
            // If we're running for these keys, then skip usual QMK processing.
            return !diablo3_key_running(keycode);
    }

#if 0
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
        if (keycode == KC_ESCAPE || keycode == QK_GESC) {
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
#endif

    // If we get here, fallback to normal key functionality.
    return true;
}

static uint32_t diablo3_press_delay(bool is_pressed) {
    return is_pressed ? (80 + (uint32_t)prng())                      // Short release
                      : (200 + (uint32_t)prng() + (uint32_t)prng()); // Long wait for press
}

uint32_t diablo3_deferred_exec_callback(uint32_t trigger_time, void *cb_arg) {
    // Work out which key we care about
    uint8_t idx = (uint8_t)(uintptr_t)cb_arg;

    // Figure out if we're doing keyup or keydown
    if (!diablo3_runtime.key_desc[idx].pressed) {
        register_code16(idx + KC_1);
        diablo3_runtime.key_desc[idx].pressed = true;
    } else {
        unregister_code16(idx + KC_1);
        diablo3_runtime.key_desc[idx].pressed = false;
    }

    // Signal the next repeat time
    return diablo3_press_delay(diablo3_runtime.key_desc[idx].pressed);
}

void disable_automatic_diablo3_key(uint16_t kc) {
    uint8_t idx = kc - KC_1;
    // If this key is disabled, kill any deferred executor if it's present.
    if (diablo3_runtime.key_desc[idx].token != INVALID_DEFERRED_TOKEN) {
        cancel_deferred_exec_advanced(d3_execs, ARRAY_SIZE(d3_execs), diablo3_runtime.key_desc[idx].token);
        diablo3_runtime.key_desc[idx].token = INVALID_DEFERRED_TOKEN;
        dprintf("[D3] Key repeat stopped for %s\n", key_name(KC_1 + idx, false));
    }

    // Release the key if it was pressed.
    if (diablo3_runtime.key_desc[idx].pressed) {
        unregister_code16(kc);
        diablo3_runtime.key_desc[idx].pressed = false;
    }
}

void enable_automatic_diablo3(void) {
    for (uint16_t kc = KC_1; kc <= KC_4; ++kc) {
        if (diablo3_key_enabled_get(kc)) {
            // If this key is enabled, but we don't yet have a deferred executor running... start one.
            uint8_t idx = kc - KC_1;
            if (diablo3_runtime.key_desc[idx].token == INVALID_DEFERRED_TOKEN) {
                diablo3_runtime.key_desc[idx].token = defer_exec_advanced(d3_execs, ARRAY_SIZE(d3_execs), diablo3_press_delay(false), diablo3_deferred_exec_callback, (void *)(uintptr_t)idx);
                dprintf("[D3] Key repeat started for %s\n", key_name(KC_1 + idx, false));
            }
        } else {
            disable_automatic_diablo3_key(kc);
        }
    }
}

void disable_automatic_diablo3(void) {
    for (uint16_t kc = KC_1; kc <= KC_4; ++kc) {
        disable_automatic_diablo3_key(kc);
    }

    diablo3_runtime.config_mode       = false;
    diablo3_runtime.last_config_press = 0;
}

void matrix_scan_diablo3(void) {
    // Run any deferred executors if they're still active.
    deferred_exec_advanced_task(d3_execs, ARRAY_SIZE(d3_execs), &d3_last_exec);
}
