// Copyright 2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "vcall.h"
#include "rv32_runner.h"

static inline uint32_t timer_read32(void) {
    return (uint32_t)_vcall0(RV32_VCALL_TIMER_READ32);
}

static inline void rgb_matrix_set_color(int index, uint8_t red, uint8_t green, uint8_t blue) {
    _vcall4(RV32_VCALL_RGB_MATRIX_SET_COLOR, index, red, green, blue);
}

void effect_init(void) {}

void effect_led(void *params, uint8_t led_index) {
    rgb_matrix_set_color(led_index, 0x00, 0x00, 60 + (timer_read32() % 1000)/8);
}
