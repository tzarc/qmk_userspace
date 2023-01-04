// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <string.h>
#include <eeprom.h>
#include <eeconfig.h>

#include "tzarc.h"

#define TZARC_EEPROM_DEBUGGING

#ifdef TZARC_EEPROM_DEBUGGING
static inline void dump_config(const char *name, struct tzarc_eeprom_cfg_t *cfg) {
    dprintf("config %s:\n", name);
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
#endif

struct tzarc_eeprom_cfg_t tzarc_eeprom_cfg;
EECONFIG_DEBOUNCE_HELPER_CHECKED(tzarc_eeprom, EECONFIG_USER_DATABLOCK, tzarc_eeprom_cfg);
bool eeconfig_check_valid_tzarc_eeprom(void) {
    return eeprom_read_dword(EECONFIG_USER) == (EECONFIG_USER_DATA_VERSION);
}
void eeconfig_post_flush_tzarc_eeprom(void) {
    eeprom_update_dword(EECONFIG_USER, (EECONFIG_USER_DATA_VERSION));

#ifdef TZARC_EEPROM_DEBUGGING
    dump_config("flush", &tzarc_eeprom_cfg);
#endif
}

void tzarc_eeprom_init(void) {
    eeconfig_init_tzarc_eeprom();

#ifdef TZARC_EEPROM_DEBUGGING
    dump_config("read", &tzarc_eeprom_cfg);
#endif
}

void tzarc_eeprom_reset(void) {
    memset(&tzarc_eeprom_cfg, 0, sizeof(tzarc_eeprom_cfg));
    eeconfig_flush_tzarc_eeprom(true);

#ifdef TZARC_EEPROM_DEBUGGING
    dump_config("reset", &tzarc_eeprom_cfg);
#endif
}

void tzarc_eeprom_mark_dirty(void) {
    eeconfig_flag_tzarc_eeprom(true);

#ifdef TZARC_EEPROM_DEBUGGING
    dump_config("dirty", &tzarc_eeprom_cfg);
#endif
}

void tzarc_eeprom_task(void) {
    eeconfig_flush_tzarc_eeprom_task(125);
}
