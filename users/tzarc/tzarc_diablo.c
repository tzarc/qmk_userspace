// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdint.h>
#include <string.h>
#include <quantum.h>
#include "deferred_exec.h"
#include "timer.h"
#include "tzarc.h"

_Static_assert(DIABLO_NUM_KEYS <= 8, "DIABLO_NUM_KEYS must be <= 8");

struct diablo_runtime_t diablo_runtime;
uint32_t                diablo_last_exec;
deferred_executor_t     diablo_executors[DIABLO_NUM_KEYS];

void tzarc_diablo_init(void) {
    diablo_automatic_stop();
}

static const uint16_t diablo_keycodes[] = {DIABLO_KEYCODES};

uint8_t diablo_keycode_to_index(uint16_t keycode) {
    for (uint8_t i = 0; i < DIABLO_NUM_KEYS; ++i) {
        if (diablo_keycodes[i] == keycode) {
            return i;
        }
    }
    return 0;
}

uint16_t diablo_index_to_keycode(uint8_t index) {
    if (index >= DIABLO_NUM_KEYS) {
        return KC_1;
    }
    return diablo_keycodes[index];
}

static struct diablo_key_desc *diablo_get_key_desc(uint16_t keycode) {
    return &diablo_runtime.key_desc[diablo_keycode_to_index(keycode)];
}

bool diablo_automatic_key_enabled(uint16_t keycode) {
    return (tzarc_eeprom_cfg.diablo_enabled_keys & (1 << diablo_keycode_to_index(keycode))) ? true : false;
}

static void diablo_automatic_key_set(uint16_t keycode, bool on) {
    if (diablo_automatic_key_enabled(keycode) != on) {
        if (on) {
            tzarc_eeprom_cfg.diablo_enabled_keys |= (1 << diablo_keycode_to_index(keycode));
        } else {
            tzarc_eeprom_cfg.diablo_enabled_keys &= ~(1 << diablo_keycode_to_index(keycode));
        }
        tzarc_eeprom_mark_dirty();
    }
    dprintf("[Diablo] Key repeat on %s: %s\n", key_name(keycode, false), on ? "on" : "off");
}

static bool diablo_key_running(uint16_t keycode) {
    return diablo_get_key_desc(keycode)->token != INVALID_DEFERRED_TOKEN;
}

static bool diablo_key_captured(uint16_t keycode) {
    for (uint8_t i = 0; i < DIABLO_NUM_KEYS; ++i) {
        if (diablo_keycodes[i] == keycode) {
            return true;
        }
    }
    return false;
}

bool diablo_automatic_active(void) {
    for (uint8_t i = 0; i < DIABLO_NUM_KEYS; ++i) {
        if (diablo_key_running(diablo_index_to_keycode(i))) {
            return true;
        }
    }
    return false;
}

bool process_record_diablo(uint16_t keycode, keyrecord_t *record) {
    // If any executors are active, capture <ESC> and prevent it from functioning.
    // On release of <ESC>, disable all executors. Effectively skips the first <ESC>.
    if (keycode == KC_ESCAPE || keycode == QK_GESC) {
        bool any_active = diablo_automatic_active();

        // Consume the <ESC> on both keydown/keyup if any executors are active.
        if (any_active || diablo_runtime.config_mode_active) {
            // Cancel any automatic keypress processing
            if (!record->event.pressed) {
                if (diablo_runtime.config_mode_active) {
                    dprintf("[Diablo] Config mode deactivated\n");
                }
                diablo_automatic_stop();
            }
            return false;
        }
    }

    // If we're running for any of the captured keys, then skip usual QMK processing.
    if (diablo_key_captured(keycode) && diablo_key_running(keycode)) {
        return false;
    }

    // Handle config logic with encoder
    switch (keycode) {
        case TZ_ENC1L:
            if (diablo_runtime.config_mode_active && !record->event.pressed) {
                diablo_runtime.config_curr_selection = (diablo_runtime.config_curr_selection + DIABLO_NUM_KEYS - 1) % DIABLO_NUM_KEYS;
                dprintf("[Diablo] Config selection: %s\n", key_name(diablo_index_to_keycode(diablo_runtime.config_curr_selection), false));
            }
            return false;
        case TZ_ENC1R:
            if (diablo_runtime.config_mode_active && !record->event.pressed) {
                diablo_runtime.config_curr_selection = (diablo_runtime.config_curr_selection + DIABLO_NUM_KEYS + 1) % DIABLO_NUM_KEYS;
                dprintf("[Diablo] Config selection: %s\n", key_name(diablo_index_to_keycode(diablo_runtime.config_curr_selection), false));
            }
            return false;
        case TZ_ENC1P:
            if (record->event.pressed) {
                diablo_runtime.last_config_press = timer_read32();
            } else {
                uint32_t now = timer_read32();
                if (TIMER_DIFF_32(now, diablo_runtime.last_config_press) > (DIABLO_CONFIG_PRESS_DELTA)) {
                    // Long press of the encoder button -- we want to go into config mode, <ESC> to exit.
                    if (!diablo_runtime.config_mode_active) {
                        dprintf("[Diablo] Config mode activated, current selection: %s\n", key_name(diablo_index_to_keycode(diablo_runtime.config_curr_selection), false));
                    }
                    diablo_automatic_stop();
                    diablo_runtime.config_mode_active = true;
                } else {
                    // Short press of the encoder button
                    if (diablo_runtime.config_mode_active) {
                        // Toggle the current selection
                        uint16_t keycode = diablo_index_to_keycode(diablo_runtime.config_curr_selection);
                        diablo_automatic_key_set(keycode, !diablo_automatic_key_enabled(keycode));
                    } else {
                        diablo_automatic_start();
                    }
                }
                diablo_runtime.last_config_press = 0;
            }
            return false;
    }

    // If we get here, fallback to normal key functionality.
    return true;
}

static uint32_t diablo_press_delay(bool is_pressed) {
    return is_pressed ? prng(110, 350)   // Short release
                      : prng(250, 1000); // Long wait for press
}

static uint32_t diablo_deferred_exec_callback(uint32_t trigger_time, void *cb_arg) {
    // Work out which key we care about
    uint8_t                 idx     = (uint8_t)(uintptr_t)cb_arg;
    uint16_t                keycode = diablo_index_to_keycode(idx);
    struct diablo_key_desc *keydesc = diablo_get_key_desc(keycode);

    // Figure out if we're doing keyup or keydown
    if (!keydesc->pressed) {
        register_code16(keycode);
        keydesc->pressed = true;
    } else {
        unregister_code16(keycode);
        keydesc->pressed = false;
    }

    // Signal the next repeat time
    return diablo_press_delay(keydesc->pressed);
}

static void diablo_automatic_stop_key(uint16_t keycode) {
    // Work out which key we care about
    struct diablo_key_desc *keydesc = diablo_get_key_desc(keycode);

    // If this key is disabled, kill any deferred executor if it's present.
    if (keydesc->token != INVALID_DEFERRED_TOKEN) {
        cancel_deferred_exec_advanced(diablo_executors, ARRAY_SIZE(diablo_executors), keydesc->token);
        keydesc->token = INVALID_DEFERRED_TOKEN;
        dprintf("[Diablo] Key repeat stopped for %s\n", key_name(keycode, false));
    }

    // Release the key if it was pressed.
    if (keydesc->pressed) {
        unregister_code16(keycode);
        keydesc->pressed = false;
    }
}

void diablo_automatic_start(void) {
    for (uint8_t i = 0; i < DIABLO_NUM_KEYS; ++i) {
        uint16_t                keycode = diablo_index_to_keycode(i);
        struct diablo_key_desc *keydesc = diablo_get_key_desc(keycode);
        if (diablo_automatic_key_enabled(keycode)) {
            // If this key is enabled, but we don't yet have a deferred executor running... start one.
            if (keydesc->token == INVALID_DEFERRED_TOKEN) {
                keydesc->token = defer_exec_advanced(diablo_executors, ARRAY_SIZE(diablo_executors), diablo_press_delay(false), diablo_deferred_exec_callback, (void *)(uintptr_t)i);
                dprintf("[Diablo] Key repeat started for %s\n", key_name(keycode, false));
            }
        } else {
            diablo_automatic_stop_key(keycode);
        }
    }
}

void diablo_automatic_stop(void) {
    for (uint8_t i = 0; i < DIABLO_NUM_KEYS; ++i) {
        uint16_t keycode = diablo_index_to_keycode(i);
        diablo_automatic_stop_key(keycode);
    }

    diablo_runtime.config_mode_active = false;
    diablo_runtime.last_config_press  = 0;
}

void matrix_scan_diablo(void) {
    // Run any deferred executors if they're still active.
    deferred_exec_advanced_task(diablo_executors, ARRAY_SIZE(diablo_executors), &diablo_last_exec);
}
