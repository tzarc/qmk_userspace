// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <stdbool.h>
#include <stdint.h>

// #include "action.h"
typedef struct keyrecord_t keyrecord_t;

typedef struct qmk_bridge_t {
    // QMK exposed APIs
    bool (*is_keyboard_master)(void);
    bool (*is_keyboard_left)(void);

    // Keymap APIs
    bool (*process_record_user)(uint16_t keycode, keyrecord_t *record);
    void (*housekeeping_task_user)(void);
} qmk_bridge_t;