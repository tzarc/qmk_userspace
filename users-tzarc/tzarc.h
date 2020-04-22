#pragma once
#include <quantum.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common layers and keys

enum { KC_CONFIG = SAFE_RANGE, TIME_RESET, TIME_EEPRST, KC_LOWER, KC_RAISE, KC_ADJUST, KC_NOMODE, KC_WIDE, KC_SCRIPT, KC_BLOCKS, KC_WOWMODE, KC_D3MODE, KEYMAP_SAFE_RANGE };
enum { LAYER_BASE, LAYER_LOWER, LAYER_RAISE, LAYER_ADJUST };

#define KC_TAB_LOWER LT(LAYER_LOWER, KC_TAB)
#define KC_APP_RAISE LT(LAYER_RAISE, KC_APP)
#define KC_CTL_ESC MT(MOD_LCTL, KC_ESC)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common functionality

extern bool     config_enabled;
extern uint16_t repeat_mode;

void tzarc_common_init(void);

uint8_t prng(void);

bool process_record_keymap(uint16_t keycode, keyrecord_t *record);
bool process_record_user(uint16_t keycode, keyrecord_t *record);

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
#define TZARC_EEPROM_MAGIC_SEED (uint8_t)((uint8_t)__TIME__[0] + (uint8_t)__TIME__[1] + (uint8_t)__TIME__[3] + (uint8_t)__TIME__[4] + (uint8_t)__TIME__[6] + (uint8_t)__TIME__[7])  // HH:MM::SS

__attribute__((packed)) struct tzarc_eeprom_cfg_t {
    uint8_t magic1;
    uint8_t wow_enabled[BITMASK_BYTES_REQUIRED(KC_0, KC_A)];
    uint8_t d3_enabled[BITMASK_BYTES_REQUIRED(KC_4, KC_1)];
    uint8_t magic2;
};

extern struct tzarc_eeprom_cfg_t tzarc_eeprom_cfg;

void tzarc_eeprom_init(void);
void tzarc_eeprom_reset(void);
void tzarc_eeprom_save(void);
void tzarc_eeprom_load(void);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WoW

#define WOW_BUTTON_COUNT (KC_0 - KC_A + 1)

struct wow_config_t {
    uint8_t  keydown[BITMASK_BYTES_REQUIRED(KC_0, KC_A)];
    uint8_t  released[BITMASK_BYTES_REQUIRED(KC_0, KC_A)];
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
    bool dummy;
};

extern struct diablo3_config_t diablo3_config;

void tzarc_diablo3_init(void);
bool process_record_diablo3(uint16_t keycode, keyrecord_t *record);
void matrix_scan_diablo3(void);
