// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#include <string.h>
#include <quantum.h>
#include <process_unicode_common.h>
#include "tzarc.h"

void register_unicode(uint32_t hex);
#define tap_unicode_glyph register_unicode

bool     config_enabled;
uint16_t typing_mode;

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

uint8_t prng(void) {
    static uint8_t s = 0xAA, a = 0;
    s ^= s << 3;
    s ^= s >> 5;
    s ^= a++ >> 2;
    return s;
}

void tap_code16_nomods(uint8_t kc) {
    uint8_t temp_mod = get_mods();
    clear_mods();
    clear_oneshot_mods();
    tap_code16(kc);
    set_mods(temp_mod);
}

void tap_unicode_glyph_nomods(uint32_t glyph) {
#ifdef UNICODE_ENABLE
    uint8_t temp_mod = get_mods();
    clear_mods();
    clear_oneshot_mods();
    tap_unicode_glyph(glyph);
    set_mods(temp_mod);
#endif
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

__attribute__((weak)) bool encoder_update_keymap(uint8_t index, bool clockwise) {
    return true;
}

__attribute__((weak)) void housekeeping_task_keymap(void) {}

void tzarc_common_init(void) {
    config_enabled = false;
    typing_mode    = KC_NOMODE;
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

typedef uint32_t (*translator_function_t)(bool is_shifted, uint32_t keycode);

#define DEFINE_UNICODE_RANGE_TRANSLATOR(translator_name, lower_alpha, upper_alpha, zero_glyph, number_one, space_glyph) \
    static inline uint32_t translator_name(bool is_shifted, uint32_t keycode) {                                         \
        switch (keycode) {                                                                                              \
            case KC_A ... KC_Z:                                                                                         \
                return (is_shifted ? upper_alpha : lower_alpha) + keycode - KC_A;                                       \
            case KC_0:                                                                                                  \
                return zero_glyph;                                                                                      \
            case KC_1 ... KC_9:                                                                                         \
                return (number_one + keycode - KC_1);                                                                   \
            case KC_SPACE:                                                                                              \
                return space_glyph;                                                                                     \
        }                                                                                                               \
        return keycode;                                                                                                 \
    }

#define DEFINE_UNICODE_LUT_TRANSLATOR(translator_name, ...)                     \
    static inline uint32_t translator_name(bool is_shifted, uint32_t keycode) { \
        static const uint32_t translation[] PROGMEM = {__VA_ARGS__};            \
        uint32_t              ret                   = keycode;                  \
        if ((keycode - KC_A) < (sizeof(translation) / sizeof(uint32_t))) {      \
            ret = pgm_read_dword(&translation[keycode - KC_A]);                 \
        }                                                                       \
        return ret;                                                             \
    }

bool process_record_glyph_replacement(uint16_t keycode, keyrecord_t *record, translator_function_t translator) {
#ifdef UNICODE_ENABLE
    uint8_t temp_mod   = get_mods();
    uint8_t temp_osm   = get_oneshot_mods();
    bool    is_shifted = (temp_mod | temp_osm) & MOD_MASK_SHIFT;
    if (((temp_mod | temp_osm) & (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI)) == 0) {
        if (KC_A <= keycode && keycode <= KC_Z) {
            if (record->event.pressed) {
                tap_unicode_glyph_nomods(translator(is_shifted, keycode));
            }
            return false;
        } else if (KC_1 <= keycode && keycode <= KC_0) {
            if (is_shifted) { // skip shifted numbers, so that we can still use symbols etc.
                return process_record_keymap(keycode, record);
            }
            if (record->event.pressed) {
                tap_unicode_glyph(translator(is_shifted, keycode));
            }
            return false;
        } else if (keycode == KC_SPACE) {
            if (record->event.pressed) {
                tap_unicode_glyph(translator(is_shifted, keycode));
            }
            return false;
        }
    }
#endif
    return process_record_keymap(keycode, record);
}

DEFINE_UNICODE_RANGE_TRANSLATOR(unicode_range_translator_wide, 0xFF41, 0xFF21, 0xFF10, 0xFF11, 0x2003);
DEFINE_UNICODE_RANGE_TRANSLATOR(unicode_range_translator_script, 0x1D4EA, 0x1D4D0, 0x1D7CE, 0x1D7C1, 0x2002);
DEFINE_UNICODE_RANGE_TRANSLATOR(unicode_range_translator_boxes, 0x1F170, 0x1F170, '0', '1', 0x2002);
DEFINE_UNICODE_RANGE_TRANSLATOR(unicode_range_translator_regional, 0x1F1E6, 0x1F1E6, '0', '1', 0x2003);

#ifndef __AVR__
DEFINE_UNICODE_LUT_TRANSLATOR(unicode_lut_translator_aussie,
                              0x0250, // a
                              'q',    // b
                              0x0254, // c
                              'p',    // d
                              0x01DD, // e
                              0x025F, // f
                              0x0183, // g
                              0x0265, // h
                              0x1D09, // i
                              0x027E, // j
                              0x029E, // k
                              'l',    // l
                              0x026F, // m
                              'u',    // n
                              'o',    // o
                              'd',    // p
                              'b',    // q
                              0x0279, // r
                              's',    // s
                              0x0287, // t
                              'n',    // u
                              0x028C, // v
                              0x028D, // w
                              0x2717, // x
                              0x028E, // y
                              'z',    // z
                              0x0269, // 1
                              0x3139, // 2
                              0x0190, // 3
                              0x3123, // 4
                              0x03DB, // 5
                              '9',    // 6
                              0x3125, // 7
                              '8',    // 8
                              '6',    // 9
                              '0'     // 0
);

bool process_record_aussie(uint16_t keycode, keyrecord_t *record) {
    bool is_shifted = (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
    if ((KC_A <= keycode) && (keycode <= KC_0)) {
        if (record->event.pressed) {
            if (!process_record_glyph_replacement(keycode, record, unicode_lut_translator_aussie)) {
                tap_code16_nomods(KC_LEFT);
                return false;
            }
        }
    } else if (record->event.pressed && keycode == KC_SPACE) {
        tap_code16_nomods(KC_SPACE);
        tap_code16_nomods(KC_LEFT);
        return false;
    } else if (record->event.pressed && keycode == KC_ENTER) {
        tap_code16_nomods(KC_END);
        tap_code16_nomods(KC_ENTER);
        return false;
    } else if (record->event.pressed && keycode == KC_HOME) {
        tap_code16_nomods(KC_END);
        return false;
    } else if (record->event.pressed && keycode == KC_END) {
        tap_code16_nomods(KC_HOME);
        return false;
    } else if (record->event.pressed && keycode == KC_BSPC) {
        tap_code16_nomods(KC_DELT);
        return false;
    } else if (record->event.pressed && keycode == KC_DELT) {
        tap_code16_nomods(KC_BSPC);
        return false;
    } else if (record->event.pressed && keycode == KC_QUOT) {
        tap_unicode_glyph_nomods(is_shifted ? 0x201E : 0x201A);
        tap_code16_nomods(KC_LEFT);
        return false;
    } else if (record->event.pressed && keycode == KC_COMMA) {
        tap_unicode_glyph_nomods(is_shifted ? '<' : 0x2018);
        tap_code16_nomods(KC_LEFT);
        return false;
    } else if (record->event.pressed && keycode == KC_DOT) {
        tap_unicode_glyph_nomods(is_shifted ? '>' : 0x02D9);
        tap_code16_nomods(KC_LEFT);
        return false;
    } else if (record->event.pressed && keycode == KC_SLASH) {
        tap_unicode_glyph_nomods(is_shifted ? 0x00BF : '/');
        tap_code16_nomods(KC_LEFT);
        return false;
    }
    return process_record_keymap(keycode, record);
}

bool process_record_zalgo(uint16_t keycode, keyrecord_t *record) {
    if ((KC_A <= keycode) && (keycode <= KC_0)) {
        if (record->event.pressed) {
            tap_code16_nomods(keycode);

            int number = (rand() % (8 + 1 - 2)) + 2;
            for (int index = 0; index < number; index++) {
                uint16_t hex = (rand() % (0x036F + 1 - 0x0300)) + 0x0300;
                tap_unicode_glyph(hex);
            }

            return false;
        }
    }
    return process_record_keymap(keycode, record);
}
#endif // __AVR__

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint32_t reset_key_timer  = 0;
    static uint32_t eeprst_key_timer = 0;

    switch (keycode) {
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

        case KC_LOWER:
            if (record->event.pressed) {
                layer_on(LAYER_LOWER);
            } else {
                layer_off(LAYER_LOWER);
            }
            return false;

        case KC_RAISE:
            if (record->event.pressed) {
                layer_on(LAYER_RAISE);
            } else {
                layer_off(LAYER_RAISE);
            }
            return false;

        case KC_ADJUST:
            if (record->event.pressed) {
                layer_on(LAYER_ADJUST);
            } else {
                layer_off(LAYER_ADJUST);
            }
            return false;

        case KC_CONFIG:
            // Config enabled while pressed
            config_enabled = record->event.pressed;
            dprintf("Config enabled: %s\n", config_enabled ? "true" : "false");
            return false;

        case KC_NOMODE:
            if (record->event.pressed) {
                if (typing_mode != KC_NOMODE) {
                    dprint("Disabling repeat mode\n");
                }
                typing_mode = keycode;
                disable_automatic_diablo3();
            }
            return false;

        case KC_WIDE:
            if (record->event.pressed) {
                if (typing_mode != KC_WIDE) {
                    dprint("Enabling wide mode\n");
                }
                typing_mode = keycode;
                disable_automatic_diablo3();
            }
            return false;

        case KC_SCRIPT:
            if (record->event.pressed) {
                if (typing_mode != KC_SCRIPT) {
                    dprint("Enabling calligraphy mode\n");
                }
                typing_mode = keycode;
                disable_automatic_diablo3();
            }
            return false;

        case KC_BLOCKS:
            if (record->event.pressed) {
                if (typing_mode != KC_BLOCKS) {
                    dprint("Enabling blocks mode\n");
                }
                typing_mode = keycode;
                disable_automatic_diablo3();
            }
            return false;

        case KC_REGIONAL:
            if (record->event.pressed) {
                if (typing_mode != KC_REGIONAL) {
                    dprint("Enabling regional mode\n");
                }
                typing_mode = keycode;
                disable_automatic_diablo3();
            }
            return false;

        case KC_AUSSIE:
            if (record->event.pressed) {
                if (typing_mode != KC_AUSSIE) {
                    dprint("Enabling aussie mode\n");
                }
                typing_mode = keycode;
                disable_automatic_diablo3();
            }
            return false;

        case KC_ZALGO:
            if (record->event.pressed) {
                if (typing_mode != KC_ZALGO) {
                    dprint("Enabling zalgo mode\n");
                }
                typing_mode = keycode;
                disable_automatic_diablo3();
            }
            return false;

        case KC_WOWMODE:
            if (record->event.pressed) {
                if (typing_mode != KC_WOWMODE) {
                    dprint("Enabling WoW repeat mode\n");
                }
                typing_mode = keycode;
                disable_automatic_diablo3();
            }
            return false;

        case KC_D3MODE:
            if (record->event.pressed) {
                if (typing_mode != KC_D3MODE) {
                    dprint("Enabling Diablo III repeat mode\n");
                }
                typing_mode = keycode;
            }
            return false;
    }

    if (typing_mode == KC_WOWMODE) {
        if ((WOW_KEY_MIN <= keycode) && (keycode <= WOW_KEY_MAX)) {
            return process_record_wow(keycode, record);
        }
    } else if (typing_mode == KC_D3MODE) {
        if (((KC_1 <= keycode) && (keycode <= KC_4)) || keycode == KC_ESCAPE || keycode == KC_GESC) {
            return process_record_diablo3(keycode, record);
        }
    } else if (typing_mode == KC_WIDE) {
        if (((KC_A <= keycode) && (keycode <= KC_0)) || keycode == KC_SPACE) {
            return process_record_glyph_replacement(keycode, record, unicode_range_translator_wide);
        }
    } else if (typing_mode == KC_SCRIPT) {
        if (((KC_A <= keycode) && (keycode <= KC_0)) || keycode == KC_SPACE) {
            return process_record_glyph_replacement(keycode, record, unicode_range_translator_script);
        }
    } else if (typing_mode == KC_BLOCKS) {
        if (((KC_A <= keycode) && (keycode <= KC_0)) || keycode == KC_SPACE) {
            return process_record_glyph_replacement(keycode, record, unicode_range_translator_boxes);
        }
    } else if (typing_mode == KC_REGIONAL) {
        if (((KC_A <= keycode) && (keycode <= KC_0)) || keycode == KC_SPACE) {
            if (!process_record_glyph_replacement(keycode, record, unicode_range_translator_regional)) {
                tap_unicode_glyph_nomods(0x200C);
                return false;
            }
        }
    }
#ifndef __AVR__
    else if (typing_mode == KC_AUSSIE) {
        return process_record_aussie(keycode, record);
    } else if (typing_mode == KC_ZALGO) {
        return process_record_zalgo(keycode, record);
    }
#endif // __AVR__

    return process_record_keymap(keycode, record);
}

void matrix_scan_user(void) {
    if (typing_mode == KC_WOWMODE) {
        matrix_scan_wow();
    } else if (typing_mode == KC_D3MODE) {
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

bool encoder_update_user(uint8_t index, bool clockwise) {
    return encoder_update_keymap(index, clockwise);
}
void housekeeping_task_user(void) {
    housekeeping_task_keymap();
}
