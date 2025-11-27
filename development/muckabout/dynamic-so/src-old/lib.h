// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <stdbool.h>
#include <stdint.h>

// #include "action.h"
typedef struct keyrecord_t keyrecord_t;

struct qmk_host_t {
    // QMK exposed APIs
    bool (*is_keyboard_master)(void);
    bool (*is_keyboard_left)(void);
};

typedef struct qmk_host_t qmk_host_t;
typedef const qmk_host_t(*host_t);

struct qmk_keymap_t {
    // Keymap APIs
    bool (*process_record_user)(uint16_t keycode, keyrecord_t *record);
    void (*housekeeping_task_user)(void);

    int  *test_data;
    int  *test_bss;
    int  *test_nonresident;
    int **test_nonresident_ptr;
};

typedef struct qmk_keymap_t qmk_keymap_t;
typedef const qmk_keymap_t(*keymap_t);
