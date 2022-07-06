// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later

#include "quantum.h"
#include "ch.h"
#include "debug.h"
#include "timer.h"
#include "print.h"

void chibi_system_halt_hook(const char* reason) {
    // re-route to QMK toolbox...
    uprintf("system halting: %s\n", reason);
}

void chibi_system_trace_hook(void* tep) {
    // re-route to QMK toolbox...
    uprintf("trace\n");
}

void chibi_debug_check_hook(const char* func, const char* condition, int value) {
    // re-route to QMK toolbox...
    uprintf("%s debug check failure: (%s) == %s\n", func, condition, value ? "true" : "false");
    // ...and hard-loop fail
    while (1) {
    }
}

void chibi_debug_assert_hook(const char* func, const char* condition, int value, const char* reason) {
    // re-route to QMK toolbox...
    uprintf("%s debug assert (%s) failure: (%s) == %s\n", func, reason, condition, value ? "true" : "false");
    // ...and hard-loop fail
    while (1) {
    }
}

#ifdef DEBUG_EEPROM_OUTPUT

#    ifdef WEAR_LEVELING_ENABLE
#        include "wear_leveling.h"
#    endif // WEAR_LEVELING_ENABLE

uint8_t prng(void) {
    static uint8_t s = 0xAA, a = 0;
    s ^= s << 3;
    s ^= s >> 5;
    s ^= a++ >> 2;
    return s;
}

void keyboard_post_init_user(void) {
    debug_enable = true;
}

void matrix_scan_user(void) {
    static uint32_t last_eeprom_access = 0;
    uint32_t        now                = timer_read32();
    if (now - last_eeprom_access > 5000) {
        dprint("reading eeprom\n");
        last_eeprom_access = now;

        union {
            uint8_t  bytes[4];
            uint32_t raw;
        } tmp;
        extern uint8_t prng(void);
        tmp.bytes[0] = prng();
        tmp.bytes[1] = prng();
        tmp.bytes[2] = prng();
        tmp.bytes[3] = prng();

        eeconfig_update_user(tmp.raw);
        uint32_t value = eeconfig_read_user();
        if (value != tmp.raw) {
            dprint("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            dprint("!! EEPROM readback mismatch!\n");
            dprint("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        }
    }

#    ifdef WEAR_LEVELING_ENABLE
    static uint32_t last_wear_leveling_init = 0;
    if (now - last_wear_leveling_init > 30000) {
        dprint("init'ing wear-leveling\n");
        last_wear_leveling_init = now;
        wear_leveling_init();
    }
#    endif // WEAR_LEVELING_ENABLE
}

#endif // DEBUG_EEPROM_OUTPUT
