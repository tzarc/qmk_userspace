// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <quantum.h>
#include <process_unicode_common.h>
#include "tzarc.h"

void register_unicode(uint32_t hex);
#define tap_unicode_glyph register_unicode

#ifdef UNICODE_ENABLE
const char *unicode_mode_name(enum unicode_input_modes mode) {
    switch (mode) {
        case UNICODE_MODE_MACOS:
            return "macOS";
        case UNICODE_MODE_LINUX:
            return "Linux";
        case UNICODE_MODE_WINDOWS:
            return "Windows";
        case UNICODE_MODE_BSD:
            return "BSD";
        case UNICODE_MODE_WINCOMPOSE:
            return "WinCompose";
        case UNICODE_MODE_EMACS:
            return "Emacs";
        default:
            return "unknown";
    }
}
#endif

void tap_code16_nomods(uint8_t kc) {
    uint8_t temp_mod = get_mods();
    clear_mods();
    clear_oneshot_mods();
    tap_code16(kc);
    set_mods(temp_mod);
}

void tap_unicode_glyph_nomods(uint32_t glyph) {
    if (glyph == 0xFFFD) {
        return;
    }

#ifdef UNICODE_ENABLE
    uint8_t temp_mod = get_mods();
    clear_mods();
    clear_oneshot_mods();
    tap_unicode_glyph(glyph);
    set_mods(temp_mod);
#endif
}

#ifdef UNICODE_ENABLE
typedef uint32_t (*translator_function_t)(bool is_shifted, uint32_t keycode);

#    define DEFINE_UNICODE_RANGE_TRANSLATOR(translator_name, lower_alpha, upper_alpha, zero_glyph, number_one, space_glyph) \
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

#    define DEFINE_UNICODE_LUT_TRANSLATOR(translator_name, ...)                     \
        static inline uint32_t translator_name(bool is_shifted, uint32_t keycode) { \
            static const uint32_t translation[] PROGMEM = {__VA_ARGS__};            \
            uint32_t              ret                   = keycode;                  \
            if ((keycode - KC_A) < (sizeof(translation) / sizeof(uint32_t))) {      \
                ret = pgm_read_dword(&translation[keycode - KC_A]);                 \
            }                                                                       \
            return ret;                                                             \
        }

bool process_record_glyph_replacement(uint16_t keycode, keyrecord_t *record, translator_function_t translator) {
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
                return true;
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
    return true;
}

DEFINE_UNICODE_RANGE_TRANSLATOR(unicode_range_translator_wide, 0xFF41, 0xFF21, 0xFF10, 0xFF11, 0x2003);
DEFINE_UNICODE_RANGE_TRANSLATOR(unicode_range_translator_script, 0x1D4EA, 0x1D4D0, 0x1D7CE, 0x1D7C1, 0x2002);
DEFINE_UNICODE_RANGE_TRANSLATOR(unicode_range_translator_boxes, 0x1F170, 0x1F170, '0', '1', 0x2002);
DEFINE_UNICODE_RANGE_TRANSLATOR(unicode_range_translator_regional, 0x1F1E6, 0x1F1E6, '0', '1', 0x2003);

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

// https://xkcd.com/3054/
DEFINE_UNICODE_LUT_TRANSLATOR(unicode_lut_translator_screamcipher,
                              0xFFFD, // a = A (no diacritic)
                              0x0307, // b = Ȧ
                              0x0327, // c = A̧
                              0x0331, // d = A̱
                              0x0301, // e = Á
                              0x032E, // f = A̮
                              0x030B, // g = A̋
                              0x0330, // h = A̰
                              0x0309, // i = Ả
                              0x0313, // j = A̓
                              0x0323, // k = Ạ
                              0x0306, // l = Ă
                              0x030C, // m = Ǎ
                              0x0302, // n = Â
                              0x030A, // o = Å
                              0x032F, // p = A̯
                              0x0324, // q = A̤
                              0x0311, // r = Ȃ
                              0x0303, // s = Ã
                              0x0304, // t = Ā
                              0x0308, // u = Ä
                              0x0300, // v = À
                              0x030F, // w = Ȁ
                              0x033D, // x = A̽
                              0x0326, // y = A̦
                              0x0338, // z = A̸
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
        tap_code16_nomods(KC_DEL);
        return false;
    } else if (record->event.pressed && keycode == KC_DEL) {
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
    return true;
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
    return true;
}

bool process_record_screamcipher(uint16_t keycode, keyrecord_t *record) {
    if ((KC_A <= keycode) && (keycode <= KC_Z)) {
        if (record->event.pressed) {
            tap_code16(S(KC_A));
            process_record_glyph_replacement(keycode, record, unicode_lut_translator_screamcipher);
            return false;
        }
    }
    return true;
}

bool process_record_unicode(uint16_t keycode, keyrecord_t *record) {
    if (typing_mode == MODE_WIDE) {
        if (((KC_A <= keycode) && (keycode <= KC_0)) || keycode == KC_SPACE) {
            return process_record_glyph_replacement(keycode, record, unicode_range_translator_wide);
        }
    } else if (typing_mode == MODE_SCRIPT) {
        if (((KC_A <= keycode) && (keycode <= KC_0)) || keycode == KC_SPACE) {
            return process_record_glyph_replacement(keycode, record, unicode_range_translator_script);
        }
    } else if (typing_mode == MODE_BLOCKS) {
        if (((KC_A <= keycode) && (keycode <= KC_0)) || keycode == KC_SPACE) {
            return process_record_glyph_replacement(keycode, record, unicode_range_translator_boxes);
        }
    } else if (typing_mode == MODE_REGIONAL) {
        if (((KC_A <= keycode) && (keycode <= KC_0)) || keycode == KC_SPACE) {
            if (!process_record_glyph_replacement(keycode, record, unicode_range_translator_regional)) {
                tap_unicode_glyph_nomods(0x200C);
                return false;
            }
        }
    } else if (typing_mode == MODE_AUSSIE) {
        return process_record_aussie(keycode, record);
    } else if (typing_mode == MODE_ZALGO) {
        return process_record_zalgo(keycode, record);
    } else if (typing_mode == MODE_SCREAM) {
        return process_record_screamcipher(keycode, record);
    }

    return true;
}
#endif // UNICODE_ENABLE
