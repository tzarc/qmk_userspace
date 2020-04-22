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

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_CONFIG:
            if (!record->event.pressed) {
                config_enabled = !config_enabled;
                dprintf("Config enabled: %s\n", config_enabled ? "true" : "false");
            }
            return false;

        case KC_NOMODE:
            if (!record->event.pressed) {
                if (repeat_mode != KC_NOMODE) {
                    dprint("Disabling repeat mode\n");
                }
                repeat_mode = keycode;
            }
            return false;

        case KC_BLOCKMODE:
            if (!record->event.pressed) {
                if (repeat_mode != KC_BLOCKMODE) {
                    dprint("Enabling block mode\n");
                }
                repeat_mode = keycode;
            }
            return false;

        case KC_WOWMODE:
            if (!record->event.pressed) {
                if (repeat_mode != KC_WOWMODE) {
                    dprint("Enabling WoW repeat mode\n");
                }
                repeat_mode = keycode;
            }
            return false;

        case KC_D3MODE:
            if (!record->event.pressed) {
                if (repeat_mode != KC_D3MODE) {
                    dprint("Enabling Diablo III repeat mode\n");
                }
                repeat_mode = keycode;
            }
            return false;
    }

    if (repeat_mode == KC_BLOCKMODE) {
        if (KC_A <= keycode && keycode <= KC_Z) {
            unicode_input_start();
            register_hex32(0x24B6 + (keycode - KC_A));  // circle-enclosed alphas
            unicode_input_finish();
            return false;
        } else if (keycode == KC_0) {
            unicode_input_start();
            register_hex32(0x24EA);  // circle-enclosed zero
            unicode_input_finish();
            return false;
        } else if (KC_1 <= keycode && keycode <= KC_9) {
            unicode_input_start();
            register_hex32(0x2460 + (keycode - KC_1));  // circle-enclosed numbers
            unicode_input_finish();
            return false;
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