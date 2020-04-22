#include <string.h>
#include <quantum.h>
#include <process_unicode_common.h>
#include "tzarc.h"

void register_hex32(uint32_t hex);

bool     config_enabled;
uint16_t repeat_mode;

uint8_t prng(void) {
    static uint8_t s = 0xAA, a = 0;
    s ^= s << 3;
    s ^= s >> 5;
    s ^= a++ >> 2;
    return s;
}

__attribute__((weak)) void eeconfig_init_keymap(void) {}

__attribute__((weak)) void keyboard_post_init_keymap(void) {}

__attribute__((weak)) bool process_record_keymap(uint16_t keycode, keyrecord_t *record) { return true; }

__attribute__((weak)) void matrix_scan_keymap(void) {}

__attribute__((weak)) layer_state_t layer_state_set_keymap(layer_state_t state) { return state; }

void tzarc_common_init(void) {
    config_enabled = false;
    repeat_mode    = KC_NOMODE;
}

void eeconfig_init_user(void) {
    set_unicode_input_mode(UC_WINC);
    tzarc_eeprom_reset();
}

void keyboard_post_init_user(void) {
    tzarc_common_init();
    tzarc_eeprom_init();
    tzarc_wow_init();
    tzarc_diablo3_init();
    tzarc_eeprom_load();
}

bool process_record_glyph_replacement(uint16_t keycode, keyrecord_t *record, uint32_t baseAlphaLower, uint32_t baseAlphaUpper, uint32_t zeroGlyph, uint32_t baseNumberOne, uint32_t spaceGlyph) {
    uint8_t temp_mod = get_mods();
    uint8_t temp_osm = get_oneshot_mods();
    if ((((temp_mod | temp_osm) & (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI))) == 0) {
        if (KC_A <= keycode && keycode <= KC_Z) {
            if (record->event.pressed) {
                clear_mods();
                clear_oneshot_mods();

                unicode_input_start();
                uint32_t base = ((temp_mod | temp_osm) & MOD_MASK_SHIFT) ? baseAlphaUpper : baseAlphaLower;
                register_hex32(base + (keycode - KC_A));
                unicode_input_finish();

                set_mods(temp_mod);
            }
            return false;
        } else if (keycode == KC_0) {
            if ((temp_mod | temp_osm) & MOD_MASK_SHIFT) {  // skip shifted numbers, so that we can still use symbols etc.
                return process_record_keymap(keycode, record);
            }
            if (record->event.pressed) {
                unicode_input_start();
                register_hex32(zeroGlyph);
                unicode_input_finish();
            }
            return false;
        } else if (KC_1 <= keycode && keycode <= KC_9) {
            if ((temp_mod | temp_osm) & MOD_MASK_SHIFT) {  // skip shifted numbers, so that we can still use symbols etc.
                return process_record_keymap(keycode, record);
            }
            if (record->event.pressed) {
                unicode_input_start();
                register_hex32(baseNumberOne + (keycode - KC_1));
                unicode_input_finish();
            }
            return false;
        } else if (keycode == KC_SPACE) {
            if (record->event.pressed) {
                unicode_input_start();
                register_hex32(spaceGlyph);  // em space
                unicode_input_finish();
            }
            return false;
        }
    }
    return process_record_keymap(keycode, record);
}

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
            if (record->event.pressed) {
                config_enabled = !config_enabled;
                dprintf("Config enabled: %s\n", config_enabled ? "true" : "false");
            }
            return false;

        case KC_NOMODE:
            if (record->event.pressed) {
                if (repeat_mode != KC_NOMODE) {
                    dprint("Disabling repeat mode\n");
                }
                repeat_mode = keycode;
            }
            return false;

        case KC_WIDE:
            if (record->event.pressed) {
                if (repeat_mode != KC_WIDE) {
                    dprint("Enabling wide mode\n");
                }
                repeat_mode = keycode;
            }
            return false;

        case KC_SCRIPT:
            if (record->event.pressed) {
                if (repeat_mode != KC_SCRIPT) {
                    dprint("Enabling calligraphy mode\n");
                }
                repeat_mode = keycode;
            }
            return false;

        case KC_BLOCKS:
            if (record->event.pressed) {
                if (repeat_mode != KC_BLOCKS) {
                    dprint("Enabling blocks mode\n");
                }
                repeat_mode = keycode;
            }
            return false;

        case KC_WOWMODE:
            if (record->event.pressed) {
                if (repeat_mode != KC_WOWMODE) {
                    dprint("Enabling WoW repeat mode\n");
                }
                repeat_mode = keycode;
            }
            return false;

        case KC_D3MODE:
            if (record->event.pressed) {
                if (repeat_mode != KC_D3MODE) {
                    dprint("Enabling Diablo III repeat mode\n");
                }
                repeat_mode = keycode;
            }
            return false;
    }

    if (repeat_mode == KC_WIDE) {
        if (((KC_A <= keycode) && (keycode <= KC_0)) || keycode == KC_SPACE) {
            return process_record_glyph_replacement(keycode, record, 0xFF41, 0xFF21, 0xFF10, 0xFF11, 0x2003);
        }
    } else if (repeat_mode == KC_SCRIPT) {
        if (((KC_A <= keycode) && (keycode <= KC_0)) || keycode == KC_SPACE) {
            return process_record_glyph_replacement(keycode, record, 0x1D4EA, 0x1D4D0, 0x1D7CE, 0x1D7C1, 0x2002);
        }
    } else if (repeat_mode == KC_BLOCKS) {
        if (((KC_A <= keycode) && (keycode <= KC_0)) || keycode == KC_SPACE) {
            return process_record_glyph_replacement(keycode, record, 0x1F170, 0x1F170, '0', '1', 0x2002);
        }
    } else if (repeat_mode == KC_WOWMODE) {
        if ((KC_A <= keycode) && (keycode <= KC_0)) {
            return process_record_wow(keycode, record);
        }
    } else if (repeat_mode == KC_D3MODE) {
        if ((KC_1 <= keycode) && (keycode <= KC_4)) {
            return process_record_diablo3(keycode, record);
        }
    }

    return process_record_keymap(keycode, record);
}

void matrix_scan_user(void) {
    if (repeat_mode == KC_WOWMODE) {
        matrix_scan_wow();
    } else if (repeat_mode == KC_D3MODE) {
        matrix_scan_diablo3();
    }

    matrix_scan_keymap();
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // Default handler for lower/raise/adjust
    state = update_tri_layer_state(state, LAYER_LOWER, LAYER_RAISE, LAYER_ADJUST);
    return layer_state_set_keymap(state);
}