// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <string.h>
#include <quantum.h>
#include "tzarc.h"

struct wow_config_t wow_config;
bool                wow_configuration_active = false;

bool wow_key_enabled_get(uint16_t keycode) {
    return BITMASK_BIT_GET(tzarc_eeprom_cfg.wow_enabled, keycode, WOW_KEY_MIN);
}
void wow_key_enabled_set(uint16_t keycode, bool on) {
    if (wow_key_enabled_get(keycode) != on) {
        BITMASK_BIT_ASSIGN(tzarc_eeprom_cfg.wow_enabled, on, keycode, WOW_KEY_MIN);
        tzarc_eeprom_mark_dirty();
    }
}

bool wow_key_keydown_get(uint16_t keycode) {
    return BITMASK_BIT_GET(wow_config.keydown, keycode, WOW_KEY_MIN);
}
void wow_key_keydown_set(uint16_t keycode, bool on) {
    BITMASK_BIT_ASSIGN(wow_config.keydown, on, keycode, WOW_KEY_MIN);
}

bool wow_key_released_get(uint16_t keycode) {
    return BITMASK_BIT_GET(wow_config.released, keycode, WOW_KEY_MIN);
}
void wow_key_released_set(uint16_t keycode, bool on) {
    BITMASK_BIT_ASSIGN(wow_config.released, on, keycode, WOW_KEY_MIN);
}

bool wow_key_auto_registered_get(uint16_t keycode) {
    return BITMASK_BIT_GET(wow_config.auto_registered, keycode, WOW_KEY_MIN);
}
void wow_key_auto_registered_set(uint16_t keycode, bool on) {
    BITMASK_BIT_ASSIGN(wow_config.auto_registered, on, keycode, WOW_KEY_MIN);
}

uint32_t wow_key_last_keydown_get(uint16_t keycode) {
    return wow_config.last_keydown[WOW_KEY_OFFSET(keycode)];
}
void wow_key_last_keydown_set(uint16_t keycode, uint32_t last_keydown) {
    wow_config.last_keydown[WOW_KEY_OFFSET(keycode)] = last_keydown;
}

uint32_t wow_key_next_trigger_get(uint16_t keycode) {
    return wow_config.next_trigger[WOW_KEY_OFFSET(keycode)];
}
void wow_key_next_trigger_set(uint16_t keycode, uint32_t next_trigger) {
    wow_config.next_trigger[WOW_KEY_OFFSET(keycode)] = next_trigger;
}

void tzarc_wow_init(void) {
    memset(&wow_config, 0, sizeof(wow_config));
}

bool process_record_wow(uint16_t keycode, keyrecord_t *record) {
    // If it's keys we don't care about, exit.
    if (!((WOW_KEY_MIN <= keycode && keycode <= WOW_KEY_MAX) || keycode == QK_GRAVE_ESCAPE || (TZ_ENC_FIRST <= keycode && keycode <= TZ_ENC_LAST))) {
        return true;
    }

    switch (keycode) {
        case KC_ESCAPE:
        case QK_GRAVE_ESCAPE:
            if (wow_configuration_active) {
                wow_configuration_active = false;
                return false;
            }
            break;
        case TZ_ENC1P:
            if (!record->event.pressed) {
                wow_configuration_active = !wow_configuration_active;
                dprintf("[WoW] Configuration mode %s\n", wow_configuration_active ? "enabled" : "disabled");
            }
            return false;
        default:
            break;
    }

    if (wow_configuration_active) {
        if (!record->event.pressed) {
            // Toggle the enabled flag for this key
            bool new_state = !wow_key_enabled_get(keycode);
            wow_key_enabled_set(keycode, new_state);
            dprintf("[WoW] Key repeat on %s: %s\n", key_name(keycode, false), new_state ? "on" : "off");
        }
        return false;
    } else {
        // Keep track of if this key is held down
        wow_key_keydown_set(keycode, record->event.pressed);

        // Fallback to the normal key if not enabled
        if (!wow_key_enabled_get(keycode)) {
            return true;
        }

        uint32_t now = timer_read32();
        if (record->event.pressed) {
            // Keydown event, clear the released flag
            wow_key_released_set(keycode, false);

            // Keep track of last keydown, as well as next trigger time
            wow_key_last_keydown_set(keycode, now);
            wow_key_next_trigger_set(keycode, now + prng(120, 350));

            // Inform the OS that we've got a keydown event
            register_code(keycode);
        } else {
            // If the release happened within the initial hold period, then stop the timer and tap the key as per normal
            if ((now < wow_key_next_trigger_get(keycode) && !wow_key_released_get(keycode)) || wow_key_auto_registered_get(keycode)) {
                unregister_code(keycode);
                wow_key_auto_registered_set(keycode, false);
            }
        }
        return false;
    }

    return true;
}

void matrix_scan_wow(void) {
    uint32_t now = timer_read32();

    // Loop through A-Z, 0-9, Enter, Esc, Backspace, Tab, Space
    for (uint16_t keycode = WOW_KEY_MIN; keycode <= WOW_KEY_MAX; ++keycode) {
        // If this key is enabled for repeat, is held down, and we've passed the trigger point...
        if (wow_key_enabled_get(keycode) && wow_key_keydown_get(keycode) && wow_key_next_trigger_get(keycode) <= now) {
            // Check if we've not yet released due to the initial hold period
            if (!wow_key_released_get(keycode)) {
                // ...if we haven't, then we release the key.
                unregister_code(keycode);
                wow_key_released_set(keycode, true);
                wow_key_auto_registered_set(keycode, false);
            } else {
                // Toggle the keypress with random timing below
                if (!wow_key_auto_registered_get(keycode)) {
                    register_code(keycode);
                    wow_key_auto_registered_set(keycode, true);
                } else {
                    unregister_code(keycode);
                    wow_key_auto_registered_set(keycode, false);
                }
            }

            // Set the next trigger.
            wow_key_next_trigger_set(keycode, now + prng(110, 250));
        }
    }
}
