// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <string.h>
#include <quantum.h>
#include "tzarc.h"

struct diablo3_config_t diablo3_config;

void tzarc_diablo3_init(void) { memset(&diablo3_config, 0, sizeof(diablo3_config)); }

bool process_record_diablo3(uint16_t keycode, keyrecord_t *record) { return process_record_keymap(keycode, record); }

void matrix_scan_diablo3(void) {}