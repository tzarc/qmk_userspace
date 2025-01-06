// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <assert.h>
#include <quantum.h>
#include <stdint.h>
#include "tzarc_layout.h"

#ifdef FILESYSTEM_ENABLE
#    include "filesystem.h"
#endif // FILESYSTEM_ENABLE

#ifndef __cplusplus
#    define static_assert _Static_assert
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Config

// Inclusive of: A-Z, 1-0, Enter, Esc, Backspace, Tab, Space, Minus, Equal
// See keycode.h
#define WOW_KEY_MIN (KC_A)
#define WOW_KEY_MAX (KC_EQUAL)

#define DIABLO_KEYCODES KC_Q, KC_1, KC_2, KC_3, KC_4
#define DIABLO_NUM_KEYS (sizeof((uint8_t[]){DIABLO_KEYCODES}) / sizeof(uint8_t))

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common functionality

#define TYPING_MODE_LIST(XM)                             \
    /* Text glyph replacement modes */                   \
    XM(MODE_NORMAL, KC_ESC, case QK_GESC:, "normal")     \
    XM(MODE_WIDE, KC_1, /* dummy */, "wide")             \
    XM(MODE_SCRIPT, KC_2, /* dummy */, "calligraphy")    \
    XM(MODE_BLOCKS, KC_3, /* dummy */, "blocks")         \
    XM(MODE_REGIONAL, KC_4, /* dummy */, "regional")     \
    XM(MODE_AUSSIE, KC_5, /* dummy */, "Aussie")         \
    XM(MODE_ZALGO, KC_6, /* dummy */, "Zalgo")           \
    /* Game-specific modes */                            \
    XM(MODE_WOW, KC_W, /* dummy */, "World of Warcraft") \
    XM(MODE_DIABLO, KC_D, /* dummy */, "Diablo")

typedef enum typing_mode_t {
#define XM(mode, keycode, extra, name) mode,
    TYPING_MODE_LIST(XM)
#undef XM
} typing_mode_t;

#ifdef UNICODE_ENABLE
extern const char *unicode_mode_name(enum unicode_input_modes mode);
#endif
extern const char *typing_mode_name(typing_mode_t mode);

extern bool          config_enabled;
extern typing_mode_t typing_mode;

void tzarc_common_init(void);

uint32_t    prng(uint32_t min, uint32_t max);
uint8_t     prng8(void);
uint16_t    prng16(void);
uint32_t    prng32(void);
const char *key_name(uint16_t keycode, bool shifted);
const char *layer_name(uint8_t layer);
const char *mod_name(uint16_t mod);

bool process_record_keymap(uint16_t keycode, keyrecord_t *record);
bool process_record_user(uint16_t keycode, keyrecord_t *record);
bool process_record_unicode(uint16_t keycode, keyrecord_t *record);

void matrix_scan_keymap(void);
void matrix_scan_user(void);

layer_state_t layer_state_set_keymap(layer_state_t state);
layer_state_t layer_state_set_user(layer_state_t state);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EEPROM

#define BITMASK_BYTES_REQUIRED(last, first) ((((last) - (first) + 1) + 7) / 8)
#define BITMASK_BYTE_INDEX(n, first) (((n) - (first)) / 8)
#define BITMASK_BIT_INDEX(n, first) (((n) - (first)) % 8)
#define BITMASK_BIT_SET(array, n, first) ((array)[BITMASK_BYTE_INDEX((n), (first))] |= 1 << BITMASK_BIT_INDEX((n), (first)))
#define BITMASK_BIT_CLEAR(array, n, first) ((array)[BITMASK_BYTE_INDEX((n), (first))] &= ~(1 << BITMASK_BIT_INDEX((n), (first))))
#define BITMASK_BIT_GET(array, n, first) (((array)[BITMASK_BYTE_INDEX((n), (first))] & (1 << BITMASK_BIT_INDEX((n), (first)))) ? 1 : 0)
#define BITMASK_BIT_ASSIGN(array, value, n, first) (value ? BITMASK_BIT_SET((array), (n), (first)) : BITMASK_BIT_CLEAR((array), (n), (first)))

struct __attribute__((packed)) tzarc_eeprom_cfg_t {
    uint8_t wow_enabled[BITMASK_BYTES_REQUIRED(WOW_KEY_MAX, WOW_KEY_MIN)];
    uint8_t diablo_enabled_keys; // 1 bit per key, DIABLO_NUM_KEYS total
};

extern struct tzarc_eeprom_cfg_t tzarc_eeprom_cfg;

// Make sure the data block we allocate actually matches the size of EEPROM used
_Static_assert(sizeof(struct tzarc_eeprom_cfg_t) <= (EECONFIG_USER_DATA_SIZE), "sizeof(struct tzarc_eeprom_cfg_t) needs to be <= EECONFIG_USER_DATA_SIZE");

void tzarc_eeprom_init(void);
void tzarc_eeprom_reset(void);
void tzarc_eeprom_mark_dirty(void);
void tzarc_eeprom_task(void);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WoW

#define WOW_KEY_OFFSET(kc) ((kc) - (WOW_KEY_MIN))
#define WOW_BUTTON_COUNT (WOW_KEY_OFFSET(WOW_KEY_MAX) + 1)

#ifdef GAME_MODES_ENABLE
struct wow_config_t {
    uint8_t  keydown[BITMASK_BYTES_REQUIRED(WOW_KEY_MAX, WOW_KEY_MIN)];
    uint8_t  released[BITMASK_BYTES_REQUIRED(WOW_KEY_MAX, WOW_KEY_MIN)];
    uint8_t  auto_registered[BITMASK_BYTES_REQUIRED(WOW_KEY_MAX, WOW_KEY_MIN)];
    uint32_t last_keydown[WOW_BUTTON_COUNT];
    uint32_t next_trigger[WOW_BUTTON_COUNT];
};

extern struct wow_config_t wow_config;

void tzarc_wow_init(void);
bool process_record_wow(uint16_t keycode, keyrecord_t *record);
void matrix_scan_wow(void);
bool wow_key_enabled_get(uint16_t keycode);
bool wow_key_keydown_get(uint16_t keycode);
#endif // GAME_MODES_ENABLE

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Diablo III

#ifdef GAME_MODES_ENABLE

struct diablo_runtime_t {
    uint32_t last_config_press;
    bool     config_mode_active;
    uint8_t  config_curr_selection;
    struct diablo_key_desc {
        deferred_token token;
        bool           pressed;
    } key_desc[DIABLO_NUM_KEYS];
};

#    define DIABLO_CONFIG_PRESS_DELTA 250

extern struct diablo_config_t  diablo_config;
extern struct diablo_runtime_t diablo_runtime;

void tzarc_diablo_init(void);
bool process_record_diablo(uint16_t keycode, keyrecord_t *record);
void matrix_scan_diablo(void);

uint8_t  diablo_keycode_to_index(uint16_t keycode);
uint16_t diablo_index_to_keycode(uint8_t index);

bool diablo_automatic_key_enabled(uint16_t keycode);
bool diablo_automatic_active(void);
void diablo_automatic_start(void);
void diablo_automatic_stop(void);
#endif // GAME_MODES_ENABLE
