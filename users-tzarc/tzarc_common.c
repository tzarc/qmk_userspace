// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <string.h>
#include <quantum.h>
#include <process_unicode_common.h>
#include "keycodes.h"
#include "tzarc.h"
#include "tzarc_layout.h"
#include "util.h"

bool          config_enabled;
typing_mode_t typing_mode;

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

#if HAL_USE_TRNG
#    include "hal_trng.h"
#    include "hal_trng_lld.h"
static bool rng_initialized = false;
static void rng_init(void) {
    if (!rng_initialized) {
        rng_initialized = true;
        trngStart(&TRNGD1, NULL);
    }
}
bool rng_generate(void *buf, size_t n) {
    rng_init();
    trngStart(&TRNGD1, NULL);
    bool err = trngGenerate(&TRNGD1, n, (uint8_t *)buf);
    trngStop(&TRNGD1);
    return err;
}
uint8_t prng8(void) {
    uint8_t ret;
    rng_generate(&ret, sizeof(ret));
    return ret;
}
uint16_t prng16(void) {
    uint16_t ret;
    rng_generate(&ret, sizeof(ret));
    return ret;
}
uint32_t prng32(void) {
    uint32_t ret;
    rng_generate(&ret, sizeof(ret));
    return ret;
}
#else  // HAL_USE_TRNG
uint8_t prng8(void) {
    static uint8_t s = 0xAA, a = 0;
    s ^= s << 3;
    s ^= s >> 5;
    s ^= a++ >> 2;
    return s;
}
uint16_t prng16(void) {
    return prng8() | (((uint16_t)prng8()) << 8);
}
uint32_t prng32(void) {
    return prng16() | (((uint32_t)prng16()) << 16);
}
#endif // HAL_USE_TRNG

uint32_t prng(uint32_t min, uint32_t max) {
    if (min > max) {
        uint32_t tmp = min;
        min          = max;
        max          = tmp;
    }
    uint32_t range = max - min;
    if (range == 0) {
        return min;
    }
    return min + (prng32() % range);
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
    set_unicode_input_mode(UNICODE_MODE_WINCOMPOSE);
#endif
    tzarc_eeprom_reset();
    eeconfig_init_keymap();
}

__attribute__((weak)) void tzarc_sendchar_hook(uint8_t c) {}

int8_t tzarc_sendchar(uint8_t c) {
    // Forward to any sort of UI, if needed.
    tzarc_sendchar_hook(c);
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
#ifdef GAME_MODES_ENABLE
    tzarc_wow_init();
    tzarc_diablo_init();
#endif // GAME_MODES_ENABLE
    keyboard_post_init_keymap();
}

#ifdef KONAMI_CODE_ENABLE
static void konami_code_handler(void) {
    dprintf("Konami code entered\n");
    wait_ms(50);
    reset_keyboard();
}

static bool process_record_konami_code(uint16_t keycode, keyrecord_t *record) {
    static uint8_t        konami_index          = 0;
    static const uint16_t konami_code[] PROGMEM = {KC_UP, KC_UP, KC_DOWN, KC_DOWN, KC_LEFT, KC_RIGHT, KC_LEFT, KC_RIGHT, KC_B, KC_A, KC_ENTER};

    if (!record->event.pressed) {
        switch (keycode) {
            case QK_MOMENTARY ... QK_MOMENTARY_MAX:
            case QK_DEF_LAYER ... QK_DEF_LAYER_MAX:
            case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX:
            case QK_ONE_SHOT_LAYER ... QK_ONE_SHOT_LAYER_MAX:
            case QK_LAYER_TAP_TOGGLE ... QK_LAYER_TAP_TOGGLE_MAX:
                // Messing with layers, ignore but don't reset the counter.
                break;
            case QK_MOD_TAP ... QK_MOD_TAP_MAX:
                return process_record_konami_code(QK_MOD_TAP_GET_TAP_KEYCODE(keycode), record);
            case QK_LAYER_TAP ... QK_LAYER_TAP_MAX:
                return process_record_konami_code(QK_LAYER_TAP_GET_TAP_KEYCODE(keycode), record);
            case QK_SWAP_HANDS ... QK_SWAP_HANDS_MAX:
                return process_record_konami_code(QK_SWAP_HANDS_GET_TAP_KEYCODE(keycode), record);
            case KC_KP_ENTER:
            case KC_RETURN:
            case QK_SPACE_CADET_RIGHT_SHIFT_ENTER:
                return process_record_konami_code(KC_ENTER, record);
            case KC_UP:
            case KC_DOWN:
            case KC_LEFT:
            case KC_RIGHT:
            case KC_B:
            case KC_A:
            case KC_ENTER:
                dprintf("Key released: %s\n", key_name(keycode, false));
                if (keycode == pgm_read_word(&konami_code[konami_index])) {
                    konami_index++;
                    if (konami_index == ARRAY_SIZE(konami_code)) {
                        konami_index = 0;
                        konami_code_handler();
                    }
                } else {
                    konami_index = 0;
                }
                break;
            default:
                konami_index = 0;
                break;
        }
    }
    return true;
}
#endif // KONAMI_CODE_ENABLE

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint32_t reset_key_timer  = 0;
    static uint32_t eeprst_key_timer = 0;

#ifdef KONAMI_CODE_ENABLE
    if (!process_record_konami_code(keycode, record)) {
        return false;
    }
#endif // KONAMI_CODE_ENABLE

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
            diablo_automatic_stop();                    \
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

#ifdef GAME_MODES_ENABLE
    if (typing_mode == MODE_WOW) {
        if (!process_record_wow(keycode, record)) {
            return false;
        }
    } else if (typing_mode == MODE_DIABLO) {
        if (!process_record_diablo(keycode, record)) {
            return false;
        }
    }
#endif // GAME_MODES_ENABLE

    return process_record_keymap(keycode, record);
}

void matrix_scan_user(void) {
#ifdef GAME_MODES_ENABLE
    if (typing_mode == MODE_WOW) {
        matrix_scan_wow();
    } else if (typing_mode == MODE_DIABLO) {
        matrix_scan_diablo();
    }
#endif // GAME_MODES_ENABLE

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
    tzarc_eeprom_task();
}
