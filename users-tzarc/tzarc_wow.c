/* Copyright 2018-2020 Nick Brassel (@tzarc)
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

#include <string.h>
#include <quantum.h>
#include "tzarc.h"

static const char wow_keycode_display_map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

char wow_key_char(uint16_t keycode) { return wow_keycode_display_map[keycode - KC_A]; }

struct wow_config_t wow_config;

bool wow_key_enabled_get(uint16_t keycode) { return BITMASK_BIT_GET(tzarc_eeprom_cfg.wow_enabled, keycode, KC_A); }
void wow_key_enabled_set(uint16_t keycode, bool on) {
    BITMASK_BIT_ASSIGN(tzarc_eeprom_cfg.wow_enabled, on, keycode, KC_A);
    tzarc_eeprom_save();
}

bool wow_key_keydown_get(uint16_t keycode) { return BITMASK_BIT_GET(wow_config.keydown, keycode, KC_A); }
void wow_key_keydown_set(uint16_t keycode, bool on) { BITMASK_BIT_ASSIGN(wow_config.keydown, on, keycode, KC_A); }

bool wow_key_released_get(uint16_t keycode) { return BITMASK_BIT_GET(wow_config.released, keycode, KC_A); }
void wow_key_released_set(uint16_t keycode, bool on) { BITMASK_BIT_ASSIGN(wow_config.released, on, keycode, KC_A); }

uint32_t wow_key_last_keydown_get(uint16_t keycode) { return wow_config.last_keydown[keycode - KC_A]; }
void     wow_key_last_keydown_set(uint16_t keycode, uint32_t last_keydown) { wow_config.last_keydown[keycode - KC_A] = last_keydown; }

uint32_t wow_key_next_trigger_get(uint16_t keycode) { return wow_config.next_trigger[keycode - KC_A]; }
void     wow_key_next_trigger_set(uint16_t keycode, uint32_t next_trigger) { wow_config.next_trigger[keycode - KC_A] = next_trigger; }

void tzarc_wow_init(void) { memset(&wow_config, 0, sizeof(wow_config)); }

bool process_record_wow(uint16_t keycode, keyrecord_t *record) {
    if (config_enabled) {
        if (!record->event.pressed) {
            // Toggle the enabled flag for this key
            bool new_state = !wow_key_enabled_get(keycode);
            wow_key_enabled_set(keycode, new_state);
            dprintf("Key repeat on %c: %s\n", wow_key_char(keycode), new_state ? "on" : "off");
        }
        return false;
    } else {
        // Fallback to the normal key if not enabled
        if (!wow_key_enabled_get(keycode)) {
            return process_record_keymap(keycode, record);
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
        return false;
    }

    return process_record_keymap(keycode, record);
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
