/* Copyright 2019 COSEYFANNITUTTI
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#include <string.h>
#include <eeprom.h>
#include <eeconfig.h>

//#define EEPROM_DEBUGGING

#define BITMASK_BYTES_REQUIRED(last, first) ((((last) - (first) + 1) + 7) / 8)
#define BITMASK_BYTE_INDEX(n, first) (((n) - (first)) / 8)
#define BITMASK_BIT_INDEX(n, first) (((n) - (first)) % 8)
#define BITMASK_BIT_SET(array, n, first) ((array)[BITMASK_BYTE_INDEX((n), (first))] |= 1 << BITMASK_BIT_INDEX((n), (first)))
#define BITMASK_BIT_CLEAR(array, n, first) ((array)[BITMASK_BYTE_INDEX((n), (first))] &= ~(1 << BITMASK_BIT_INDEX((n), (first))))
#define BITMASK_BIT_GET(array, n, first) (((array)[BITMASK_BYTE_INDEX((n), (first))] & (1 << BITMASK_BIT_INDEX((n), (first)))) ? 1 : 0)
#define BITMASK_BIT_ASSIGN(array, value, n, first) (value ? BITMASK_BIT_SET((array), (n), (first)) : BITMASK_BIT_CLEAR((array), (n), (first)))

enum { _BASE, _FUNC1 };

enum {
    CONFIG = SAFE_RANGE,
    NOMODE,
    WOWMODE,
    D3MODE,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common functionality

static bool     config_enabled = false;
static uint16_t repeat_mode    = NOMODE;

uint8_t prng(void) {
    static uint8_t s = 0xAA, a = 0;
    s ^= s << 3;
    s ^= s >> 5;
    s ^= a++ >> 2;
    return s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EEPROM functionality

#define EEPROM_LOCATION ((uint8_t *)EECONFIG_SIZE)
#define EEPROM_MAGIC_SEED (uint8_t)((uint8_t)__TIME__[0] + (uint8_t)__TIME__[1] + (uint8_t)__TIME__[3] + (uint8_t)__TIME__[4] + (uint8_t)__TIME__[6] + (uint8_t)__TIME__[7])  // HH:MM::SS

__attribute__((packed)) struct eeprom_cfg_t {
    uint8_t magic1;
    uint8_t wow_enabled[BITMASK_BYTES_REQUIRED(KC_0, KC_A)];
    uint8_t d3_enabled[BITMASK_BYTES_REQUIRED(KC_4, KC_1)];
    uint8_t magic2;
};
static struct eeprom_cfg_t eeprom_cfg;

#ifdef EEPROM_DEBUGGING
void dump_config(const char *name, struct eeprom_cfg_t *cfg) {
    dprintf("config %s:\n", name);
    uint8_t *p = (uint8_t *)cfg;
    for (int i = 0; i < sizeof(struct eeprom_cfg_t); ++i) {
        if (i % 8 == 0) {
            dprint("| ");
        }
        dprintf("%02X ", (int)p[i]);
    }
    dprint("|\n");
}
#endif

uint8_t eeprom_calculate_magic(struct eeprom_cfg_t *cfg) {
    uint8_t magic = EEPROM_MAGIC_SEED;

    for (size_t i = 0; i < sizeof(cfg->wow_enabled); ++i) {
        magic += cfg->wow_enabled[i];
    }

    for (size_t i = 0; i < sizeof(cfg->d3_enabled); ++i) {
        magic += cfg->d3_enabled[i];
    }

    return magic;
}

bool eeprom_valid_magic(struct eeprom_cfg_t *cfg) {
    uint8_t test = eeprom_calculate_magic(cfg);

#ifdef EEPROM_DEBUGGING
    dprintf("test magic: expected=0x%02X, actual=0x%02X, inverse expected=0x%02X, actual=0x%02X\n", (int)test, (int)cfg->magic1, (int)((uint8_t)(~test)), (int)cfg->magic2);
#endif

    return (cfg->magic1 == test) && (cfg->magic2 == ((uint8_t)~test));
}

void eeprom_update_magic(struct eeprom_cfg_t *cfg) {
    cfg->magic1 = eeprom_calculate_magic(cfg);
    cfg->magic2 = ~cfg->magic1;
}

bool eeprom_data_matches(struct eeprom_cfg_t *cfg) {
    struct eeprom_cfg_t temp;
    eeprom_read_block(&temp, EEPROM_LOCATION, sizeof(temp));

    if (memcmp(temp.wow_enabled, cfg->wow_enabled, sizeof(temp.wow_enabled)) != 0) {
        return false;
    }

    if (memcmp(temp.d3_enabled, cfg->d3_enabled, sizeof(temp.d3_enabled)) != 0) {
        return false;
    }

    return true;
}

void eeprom_reset(void) {
    memset(&eeprom_cfg, 0, sizeof(eeprom_cfg));
    eeprom_update_magic(&eeprom_cfg);

#ifdef EEPROM_DEBUGGING
    dump_config("reset", &eeprom_cfg);
#endif

    eeprom_write_block(&eeprom_cfg, EEPROM_LOCATION, sizeof(eeprom_cfg));
}

void eeprom_save(void) {
    if (!eeprom_data_matches(&eeprom_cfg)) {
        eeprom_update_magic(&eeprom_cfg);

#ifdef EEPROM_DEBUGGING
        dump_config("save", &eeprom_cfg);
#endif

        eeprom_write_block(&eeprom_cfg, EEPROM_LOCATION, sizeof(eeprom_cfg));
    }
}

void eeprom_load(void) {
    eeprom_read_block(&eeprom_cfg, EEPROM_LOCATION, sizeof(eeprom_cfg));

#ifdef EEPROM_DEBUGGING
    dump_config("read", &eeprom_cfg);
#endif

    if (!eeprom_valid_magic(&eeprom_cfg)) {
        eeprom_reset();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WoW functionality

#define WOW_BUTTON_COUNT (KC_0 - KC_A + 1)

static const char wow_keycode_display_map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

char wow_key_char(uint16_t keycode) { return wow_keycode_display_map[keycode - KC_A]; }

struct wow_config_t {
    uint8_t  keydown[BITMASK_BYTES_REQUIRED(KC_0, KC_A)];
    uint8_t  released[BITMASK_BYTES_REQUIRED(KC_0, KC_A)];
    uint32_t last_keydown[WOW_BUTTON_COUNT];
    uint32_t next_trigger[WOW_BUTTON_COUNT];
};
static struct wow_config_t wow_config;

bool wow_key_enabled_get(uint16_t keycode) { return BITMASK_BIT_GET(eeprom_cfg.wow_enabled, keycode, KC_A); }
void wow_key_enabled_set(uint16_t keycode, bool on) {
    BITMASK_BIT_ASSIGN(eeprom_cfg.wow_enabled, on, keycode, KC_A);
    eeprom_save();
}

bool wow_key_keydown_get(uint16_t keycode) { return BITMASK_BIT_GET(wow_config.keydown, keycode, KC_A); }
void wow_key_keydown_set(uint16_t keycode, bool on) { BITMASK_BIT_ASSIGN(wow_config.keydown, on, keycode, KC_A); }

bool wow_key_released_get(uint16_t keycode) { return BITMASK_BIT_GET(wow_config.released, keycode, KC_A); }
void wow_key_released_set(uint16_t keycode, bool on) { BITMASK_BIT_ASSIGN(wow_config.released, on, keycode, KC_A); }

uint32_t wow_key_last_keydown_get(uint16_t keycode) { return wow_config.last_keydown[keycode - KC_A]; }
void     wow_key_last_keydown_set(uint16_t keycode, uint32_t last_keydown) { wow_config.last_keydown[keycode - KC_A] = last_keydown; }

uint32_t wow_key_next_trigger_get(uint16_t keycode) { return wow_config.next_trigger[keycode - KC_A]; }
void     wow_key_next_trigger_set(uint16_t keycode, uint32_t next_trigger) { wow_config.next_trigger[keycode - KC_A] = next_trigger; }

bool process_record_wow(uint16_t keycode, keyrecord_t *record) {
    if (config_enabled) {
        if (!record->event.pressed) {
            // Toggle the enabled flag for this key
            wow_key_enabled_set(keycode, !wow_key_enabled_get(keycode));
        }
    } else {
        // Fallback to the normal key if not enabled
        if (!wow_key_enabled_get(keycode)) {
            return true;
        }

        uint32_t now = timer_read32();
        if (record->event.pressed) {
            // Keydown event
            wow_key_keydown_set(keycode, true);
            wow_key_released_set(keycode, false);

            // Keep track of last keydown, as well as next trigger time
            wow_key_last_keydown_set(keycode, now);
            wow_key_next_trigger_set(keycode, now + 128 + (prng() % 128));

            // Inform the OS that we've got a keydown event
            register_code(keycode);
        } else {
            // Keyup event
            wow_key_keydown_set(keycode, false);

            // If the release happened within the initial hold period, then stop the timer and tap the key as per normal
            if (now < wow_key_next_trigger_get(keycode) && !wow_key_released_get(keycode)) {
                unregister_code(keycode);
            }
        }
    }

    return false;
}

void matrix_scan_wow(void) {
    uint32_t now = timer_read32();

    // Loop through A-Z, 0-9
    for (uint16_t keycode = KC_A; keycode <= KC_0; ++keycode) {
        // If this key is enabled for repeat, is held down, and we've passed the trigger point...
        if (wow_key_enabled_get(keycode) && wow_key_keydown_get(keycode) && wow_key_next_trigger_get(keycode) <= now) {
            // Check if we've not yet released due to the initial hold period
            if (!wow_key_released_get(keycode)) {
                // ...if we haven't, then we release the key.
                unregister_code(keycode);
                wow_key_released_set(keycode, true);
            } else {
                // ...otherwise, tap the key.
                tap_code(keycode);
            }

            // Set the next trigger.
            wow_key_next_trigger_set(keycode, now + 128 + (prng() % 128));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Diablo III functionality
struct d3_config_t {};
static struct d3_config_t d3_config;

bool process_record_d3(uint16_t keycode, keyrecord_t *record) { return true; }

void matrix_scan_d3(void) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base keymaps
#define TABFN LT(_FUNC1, KC_TAB)
#define APPFN LT(_FUNC1, KC_APP)
#define CTLCAPS CTL_T(KC_CAPS)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = LAYOUT_tkl_ansi(
      KC_ESC,           KC_F1,   KC_F2,   KC_F3,  KC_F4,  KC_F5,  KC_F6,  KC_F7,  KC_F8,   KC_F9,   KC_F10,   KC_F11,  KC_F12,   KC_PSCR,  KC_SLCK, KC_PAUS,
      KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,   KC_5,   KC_6,   KC_7,   KC_8,   KC_9,    KC_0,    KC_MINS,  KC_EQL,  KC_BSPC,  KC_INS,   KC_HOME, KC_PGUP,
      TABFN,   KC_Q,    KC_W,    KC_E,    KC_R,   KC_T,   KC_Y,   KC_U,   KC_I,   KC_O,    KC_P,    KC_LBRC,  KC_RBRC, KC_BSLS,  KC_DEL,   KC_END,  KC_PGDN,
      KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,   KC_G,   KC_H,   KC_J,   KC_K,   KC_L,    KC_SCLN, KC_QUOT,  KC_ENT,
      KC_LSFT,          KC_Z,    KC_X,    KC_C,   KC_V,   KC_B,   KC_N,   KC_M,   KC_COMM, KC_DOT,  KC_SLSH,  KC_RSFT,                     KC_UP,
      KC_LCTL, KC_LGUI, KC_LALT,                          KC_SPC,                 KC_RALT, KC_RGUI, APPFN,    KC_RCTL,            KC_LEFT, KC_DOWN, KC_RGHT),

  [_FUNC1] = LAYOUT_tkl_ansi(
      NOMODE,           KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS,
      CONFIG,  WOWMODE, D3MODE,  KC_TRNS, KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS, KC_TRNS, EEP_RST, RESET,  KC_TRNS,KC_TRNS,KC_P7,  KC_P8,   KC_P9,  KC_TRNS, KC_PSLS,  KC_PAST, KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS,
      KC_CAPS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS,KC_TRNS,KC_P4,  KC_P5,   KC_P6,  KC_TRNS, KC_TRNS,  KC_TRNS,
      KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS,KC_TRNS,KC_P0,  KC_P1,   KC_P2,  KC_P3,   KC_TRNS,  KC_PMNS,                     KC_VOLU,
      KC_TRNS, KC_TRNS, KC_TRNS,                          KC_TRNS,                KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,            KC_TRNS, KC_VOLD, KC_TRNS),
};
// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CONFIG:
            if (!record->event.pressed) {
                config_enabled = !config_enabled;
            }
            return false;

        case NOMODE:
        case WOWMODE:
        case D3MODE:
            if (!record->event.pressed) {
                repeat_mode = keycode;
            }
            return false;
    }

    if (repeat_mode == WOWMODE) {
        if ((KC_A <= keycode) && (keycode <= KC_0)) {
            return process_record_wow(keycode, record);
        }
    } else if (repeat_mode == D3MODE) {
        if ((KC_1 <= keycode) && (keycode <= KC_4)) {
            return process_record_d3(keycode, record);
        }
    }

    return true;
}

void eeconfig_init_user(void) { eeprom_reset(); }

void keyboard_post_init_user(void) {
    // debug_enable   = true;
    // debug_matrix   = true;
    // debug_keyboard = true;
    // debug_mouse    = true;

    config_enabled = false;
    repeat_mode    = NOMODE;
    memset(&eeprom_cfg, 0, sizeof(eeprom_cfg));
    memset(&wow_config, 0, sizeof(wow_config));
    memset(&d3_config, 0, sizeof(d3_config));
    eeprom_load();
}

void matrix_scan_user(void) {
    if (repeat_mode == WOWMODE) {
        matrix_scan_wow();
    } else if (repeat_mode == D3MODE) {
        matrix_scan_d3();
    }
}