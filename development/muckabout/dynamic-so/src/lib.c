// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#if 0
typedef struct effect_api_t {
    void (*init)(void *params);
    void (*begin_iter)(void *params, uint8_t led_min, uint8_t led_max);
    void (*led)(void **params, uint8_t led_index);
    void (*end_iter)(void *params);
    void (*deinit)(void *params);
} effect_api_t;

static void effect_init(void *params) {
}

static void effect_begin_iter(void *params, uint8_t led_min, uint8_t led_max) {
}

static void effect_led(void **params, uint8_t led_index) {
}

static effect_api_t effect_api = {
    .init = effect_init,
    .begin_iter = effect_begin_iter,
    .led = effect_led,
    .end_iter = NULL,
    .deinit = NULL,
};

effect_api_t *get_effect_api(void) {
    return &effect_api;
}

#else

#    include "lib.h"

static int    chunder_data = 5;
static int    chunder_bss;
static host_t qmk;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return qmk->is_keyboard_left() || qmk->is_keyboard_master();
}

void __attribute__((weak)) housekeeping_task_user(void) {
    process_record_user(0, 0);
}

static const qmk_keymap_t keymap = {
    .process_record_user    = process_record_user,
    .housekeeping_task_user = housekeeping_task_user,
    .test_data              = &chunder_data,
    .test_bss               = &chunder_bss,
};

__attribute__((constructor(101))) void init101(void) {
    chunder_data = 101;
}

bool keymap_init(host_t qmk_in, keymap_t *keymap_out) {
    qmk         = qmk_in;
    *keymap_out = &keymap;
    return true;
}

#endif
