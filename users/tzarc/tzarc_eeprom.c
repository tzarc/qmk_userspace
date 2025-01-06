// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <string.h>
#include <eeprom.h>
#include <eeconfig.h>

#include "tzarc.h"

#define TZARC_EEPROM_DEBUGGING

#if defined(CONSOLE_ENABLE) && defined(TZARC_EEPROM_DEBUGGING)
#    define dump_config(...) dump_config_impl(__VA_ARGS__)
static inline void dump_config_impl(const char *name, struct tzarc_eeprom_cfg_t *cfg) {
    dprintf("EEPROM config %s:\n", name);
    uint8_t *p = (uint8_t *)cfg;
    (void)p;
    for (int i = 0; i < sizeof(struct tzarc_eeprom_cfg_t); ++i) {
        if (i % 8 == 0) {
            dprint("| ");
        }
        dprintf("%02X ", (int)p[i]);
    }
    dprint("|\n");
}
#else
#    define dump_config(...) \
        do {                 \
        } while (0)
#endif

static bool               tzarc_eeprom_dirty = false;
struct tzarc_eeprom_cfg_t tzarc_eeprom_cfg;
#ifndef NVM_DRIVER
EECONFIG_DEBOUNCE_HELPER_CHECKED(tzarc_eeprom, EECONFIG_USER_DATABLOCK, tzarc_eeprom_cfg);
#endif // NVM_DRIVER

void tzarc_eeprom_init(void) {
#ifdef NVM_DRIVER
    eeconfig_read_user_datablock(&tzarc_eeprom_cfg, 0, sizeof(tzarc_eeprom_cfg));
#else  // NVM_DRIVER
    eeprom_read_block(&tzarc_eeprom_cfg, (void *)EECONFIG_USER_DATABLOCK, sizeof(tzarc_eeprom_cfg));
#endif // NVM_DRIVER
    dump_config("read", &tzarc_eeprom_cfg);
}

void tzarc_eeprom_reset(void) {
    memset(&tzarc_eeprom_cfg, 0, sizeof(tzarc_eeprom_cfg));
#ifdef NVM_DRIVER
    eeconfig_update_user_datablock(&tzarc_eeprom_cfg, 0, sizeof(tzarc_eeprom_cfg));
#else  // NVM_DRIVER
    eeprom_update_block(&tzarc_eeprom_cfg, (void *)EECONFIG_USER_DATABLOCK, sizeof(tzarc_eeprom_cfg));
#endif // NVM_DRIVER
    dump_config("reset", &tzarc_eeprom_cfg);
}

void tzarc_eeprom_mark_dirty(void) {
    tzarc_eeprom_dirty = true;
}

void tzarc_eeprom_task(void) {
    static uint32_t last_update = 0;
    if (timer_elapsed(last_update) < 125) {
        return;
    }

    if (!tzarc_eeprom_dirty) {
        return;
    }
    tzarc_eeprom_dirty = false;

#ifdef NVM_DRIVER
    eeconfig_update_user_datablock(&tzarc_eeprom_cfg, 0, sizeof(tzarc_eeprom_cfg));
#else  // NVM_DRIVER
    eeprom_update_block(&tzarc_eeprom_cfg, (void *)EECONFIG_USER_DATABLOCK, sizeof(tzarc_eeprom_cfg));
#endif // NVM_DRIVER
    dump_config("dirty", &tzarc_eeprom_cfg);
}
