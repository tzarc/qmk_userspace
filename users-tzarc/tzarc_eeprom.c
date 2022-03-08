// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <string.h>
#include <eeprom.h>
#include <eeconfig.h>

#include "tzarc.h"

// #define TZARC_EEPROM_DEBUGGING

struct tzarc_eeprom_cfg_t tzarc_eeprom_cfg;

#ifdef TZARC_EEPROM_DEBUGGING
static inline void dump_config(const char *name, struct tzarc_eeprom_cfg_t *cfg) {
    dprintf("config %s:\n", name);
    uint8_t *p = (uint8_t *)cfg;
    for (int i = 0; i < sizeof(struct tzarc_eeprom_cfg_t); ++i) {
        if (i % 8 == 0) {
            dprint("| ");
        }
        dprintf("%02X ", (int)p[i]);
    }
    dprint("|\n");
}
#endif

void tzarc_eeprom_init(void) {
    memset(&tzarc_eeprom_cfg, 0, sizeof(tzarc_eeprom_cfg));
}

static uint8_t tzarc_eeprom_calculate_magic(struct tzarc_eeprom_cfg_t *cfg) {
    uint8_t magic = TZARC_EEPROM_MAGIC_SEED;

    for (size_t i = 0; i < sizeof(cfg->wow_enabled); ++i) {
        magic += cfg->wow_enabled[i];
    }

    for (size_t i = 0; i < sizeof(cfg->d3_enabled); ++i) {
        magic += cfg->d3_enabled[i];
    }

    return magic;
}

static bool tzarc_eeprom_valid_magic(struct tzarc_eeprom_cfg_t *cfg) {
    uint8_t test = tzarc_eeprom_calculate_magic(cfg);

#ifdef TZARC_EEPROM_DEBUGGING
    dprintf("test magic: expected=0x%02X, actual=0x%02X, inverse expected=0x%02X, actual=0x%02X\n", (int)test, (int)cfg->magic1, (int)((uint8_t)(~test)), (int)cfg->magic2);
#endif

    return (cfg->magic1 == test) && (cfg->magic2 == ((uint8_t)~test));
}

void tzarc_eeprom_update_magic(struct tzarc_eeprom_cfg_t *cfg) {
    cfg->magic1 = tzarc_eeprom_calculate_magic(cfg);
    cfg->magic2 = ~cfg->magic1;
}

bool tzarc_eeprom_data_matches(struct tzarc_eeprom_cfg_t *cfg) {
    struct tzarc_eeprom_cfg_t temp;
    eeprom_read_block(&temp, TZARC_EEPROM_LOCATION, sizeof(temp));

    const uint8_t *startCfg = (const uint8_t *)cfg + sizeof(temp.magic1);
    const uint8_t *startTmp = ((const uint8_t *)&temp) + sizeof(temp.magic1);
    size_t         count    = sizeof(temp) - sizeof(temp.magic1) - sizeof(temp.magic2);
    return (memcmp(startCfg, startTmp, count) == 0);
}

void tzarc_eeprom_reset(void) {
    memset(&tzarc_eeprom_cfg, 0, sizeof(tzarc_eeprom_cfg));
    tzarc_eeprom_update_magic(&tzarc_eeprom_cfg);

#ifdef TZARC_EEPROM_DEBUGGING
    dump_config("reset", &tzarc_eeprom_cfg);
#endif

    eeprom_write_block(&tzarc_eeprom_cfg, TZARC_EEPROM_LOCATION, sizeof(tzarc_eeprom_cfg));
}

void tzarc_eeprom_save(void) {
    if (!tzarc_eeprom_data_matches(&tzarc_eeprom_cfg)) {
        tzarc_eeprom_update_magic(&tzarc_eeprom_cfg);

#ifdef TZARC_EEPROM_DEBUGGING
        dump_config("save", &tzarc_eeprom_cfg);
#endif

        eeprom_write_block(&tzarc_eeprom_cfg, TZARC_EEPROM_LOCATION, sizeof(tzarc_eeprom_cfg));
    }
}

void tzarc_eeprom_load(void) {
    eeprom_read_block(&tzarc_eeprom_cfg, TZARC_EEPROM_LOCATION, sizeof(tzarc_eeprom_cfg));

#ifdef TZARC_EEPROM_DEBUGGING
    dump_config("read", &tzarc_eeprom_cfg);
#endif

    if (!tzarc_eeprom_valid_magic(&tzarc_eeprom_cfg)) {
        tzarc_eeprom_reset();
    }
}
