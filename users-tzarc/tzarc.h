// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <assert.h>
#include <quantum.h>
#include "tzarc_layout.h"

#ifndef __cplusplus
#    define static_assert _Static_assert
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Config

// Inclusive of: A-Z, 1-0, Enter, Esc, Backspace, Tab, Space, Minus, Equal
// See keycode.h
#define WOW_KEY_MIN (KC_A)
#define WOW_KEY_MAX (KC_EQUAL)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common functionality

#define TYPING_MODE_LIST(XM)                             \
    /* Text glyph replacement modes */                   \
    XM(MODE_NORMAL, KC_ESC, case KC_GESC:, "normal")     \
    XM(MODE_WIDE, KC_1, /* dummy */, "wide")             \
    XM(MODE_SCRIPT, KC_2, /* dummy */, "calligraphy")    \
    XM(MODE_BLOCKS, KC_3, /* dummy */, "blocks")         \
    XM(MODE_REGIONAL, KC_4, /* dummy */, "regional")     \
    XM(MODE_AUSSIE, KC_5, /* dummy */, "Aussie")         \
    XM(MODE_ZALGO, KC_6, /* dummy */, "Zalgo")           \
    /* Game-specific modes */                            \
    XM(MODE_WOW, KC_W, /* dummy */, "World of Warcraft") \
    XM(MODE_D3, KC_D, /* dummy */, "Diablo 3")

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

uint8_t     prng(void);
const char *key_name(uint16_t keycode, bool shifted);

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

#define TZARC_EEPROM_LOCATION ((uint8_t *)EECONFIG_SIZE)
#define TZARC_EEPROM_MAGIC_SEED (uint8_t)((uint8_t)__TIME__[0] + (uint8_t)__TIME__[1] + (uint8_t)__TIME__[3] + (uint8_t)__TIME__[4] + (uint8_t)__TIME__[6] + (uint8_t)__TIME__[7]) // HH:MM::SS

__attribute__((packed)) struct tzarc_eeprom_cfg_t {
    uint8_t  magic1;
    uint8_t  wow_enabled[BITMASK_BYTES_REQUIRED(WOW_KEY_MAX, WOW_KEY_MIN)];
    uint16_t d3_delays[4]; // KC_1 ... KC_4
    uint8_t  magic2;
};

extern struct tzarc_eeprom_cfg_t tzarc_eeprom_cfg;

// Make sure if/when we ever use VIA we have a block of settings that still fits
static_assert(sizeof(struct tzarc_eeprom_cfg_t) <= TZARC_EEPROM_ALLOCATION, "EEPROM settings greater than TZARC_EEPROM_ALLOCATION, need to change users/tzarc/config.h");

void tzarc_eeprom_init(void);
void tzarc_eeprom_reset(void);
void tzarc_eeprom_save(void);
void tzarc_eeprom_load(void);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WoW

#define WOW_KEY_OFFSET(kc) ((kc) - (WOW_KEY_MIN))
#define WOW_BUTTON_COUNT (WOW_KEY_OFFSET(WOW_KEY_MAX) + 1)

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Diablo III

struct diablo3_config_t {
    uint8_t keys_activated[1]; // 4 bits required for KC_1 ... KC_4
};

extern struct diablo3_config_t diablo3_config;

void tzarc_diablo3_init(void);
bool process_record_diablo3(uint16_t keycode, keyrecord_t *record);
void matrix_scan_diablo3(void);
void disable_automatic_diablo3(void);
