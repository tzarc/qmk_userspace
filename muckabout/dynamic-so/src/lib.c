// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "lib.h"
#include "keycodes.h"

static int    chunder_data = 5;
static int    chunder_bss;
static int    chunder_nonresident __attribute__((section(".nonresident.chunder1")))     = 0x12345678;
static int*   chunder_nonresident_ptr __attribute__((section(".nonresident.chunder2"))) = &chunder_nonresident;
static host_t qmk;

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    return qmk->is_keyboard_left() || qmk->is_keyboard_master();
}

void housekeeping_task_user(void) {}

static const qmk_keymap_t keymap = {
    .process_record_user    = process_record_user,
    .housekeeping_task_user = housekeeping_task_user,
    .test_data              = &chunder_data,
    .test_bss               = &chunder_bss,
    .test_nonresident       = &chunder_nonresident,
    .test_nonresident_ptr   = &chunder_nonresident_ptr,
};

bool __attribute__((section(".keymap_init"))) keymap_init(host_t qmk_in, keymap_t* keymap_out) {
    qmk         = qmk_in;
    *keymap_out = &keymap;
    return true;
}