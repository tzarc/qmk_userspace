// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdint.h>
#include <string.h>
#include <quantum.h>
#include <process_unicode_common.h>
#include "timer.h"
#include "tzarc.h"
#include "tzarc_layout.h"

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

#if __has_include("prng.h")
#    include "prng.h"
#else // __has_include("prng.h")
#    if HAL_USE_TRNG
#        include "hal_trng.h"
#        include "hal_trng_lld.h"
static bool prng_initialized = false;
static void prng_init(void) {
    if (!prng_initialized) {
        prng_initialized = true;
        trngStart(&TRNGD1, NULL);
    }
}
bool prng_generate(void *buf, size_t n) {
    prng_init();
    bool err = trngGenerate(&TRNGD1, n, (uint8_t *)buf);
    return err;
}
uint8_t prng8(void) {
    uint8_t ret;
    prng_generate(&ret, sizeof(ret));
    return ret;
}
#    else // HAL_USE_TRNG
// https://filterpaper.github.io/prng.html --
// https://www.pcg-random.org/posts/bob-jenkins-small-prng-passes-practrand.html
#        define rot8(x, k) (((x) << (k)) | ((x) >> (8 - (k))))
uint8_t prng8(void) {
    static uint8_t a = 0xf1;
    static uint8_t b = 0xee, c = 0xee, d = 0xee;

    uint8_t e = a - rot8(b, 1);
    a         = b ^ rot8(c, 4);
    b         = c + d;
    c         = d + e;
    return d  = e + a;
}
bool prng_generate(void *buf, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        ((uint8_t *)buf)[i] = prng8();
    }
    return true;
}
#    endif // HAL_USE_TRNG
uint16_t prng16(void) {
    uint16_t r;
    prng_generate(&r, sizeof(r));
    return r;
}
uint32_t prng32(void) {
    uint32_t r;
    prng_generate(&r, sizeof(r));
    return r;
}
uint64_t prng64(void) {
    uint64_t r;
    prng_generate(&r, sizeof(r));
    return r;
}
#endif     // __has_include("prng.h")

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

__attribute__((weak)) bool pre_process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    return true;
}
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

void konami_code_handler(void) {
    dprintf("Konami code entered\n");
    wait_ms(50);
    reset_keyboard();
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

#ifndef GAME_MODES_ENABLE
void diablo_automatic_stop(void) {}
#endif // GAME_MODES_ENABLE

bool remember_last_key_user(uint16_t keycode, keyrecord_t *record, uint8_t *remembered_mods) {
    if (keycode >= TZ_REP_2 && keycode <= TZ_REP_5) {
        return false;
    }
    return true;
};

bool pre_process_record_user(uint16_t keycode, keyrecord_t *record) {
    return pre_process_record_keymap(keycode, record);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint32_t reset_key_timer  = 0;
    static uint32_t eeprst_key_timer = 0;

    bool is_shifted = (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
    (void)is_shifted;
    // dprintf("Keycode: %s, pressed: %s, shifted: %s\n", key_name(keycode, is_shifted), record->event.pressed ? "true" : "false", is_shifted ? "true" : "false");

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
                    mcu_reset();
                }
            }
            return false;

        case TZ_REP_2 ... TZ_REP_5:
            if (record->event.pressed) {
                for (int i = TZ_REP_2 - 1; i <= keycode; i++) {
                    tap_code16(get_last_keycode());
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

__attribute__((weak)) void eeprom_driver_flush(void) {}

void housekeeping_task_user(void) {
    static uint32_t last_eeprom_flush = 0;
    if (timer_elapsed32(last_eeprom_flush) > 1000) {
        last_eeprom_flush = timer_read32();
        eeprom_driver_flush(); // To be moved to a more appropriate location.
    }

    housekeeping_task_keymap();
    tzarc_eeprom_task();
}
