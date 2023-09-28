// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "lib.h"
#include "keycodes.h"

static qmk_bridge_t* qmk;
static int           chunder = 5;

static bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    return qmk->is_keyboard_left() || qmk->is_keyboard_master();
}

static void housekeeping_task_user(void) {}

bool keymap_init(qmk_bridge_t* qmk_in) {
    qmk                         = qmk_in;
    qmk->test                   = &chunder;
    qmk->process_record_user    = process_record_user;
    qmk->housekeeping_task_user = housekeeping_task_user;
    return true;
}
