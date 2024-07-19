// Copyright 2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "ecall.h"
#include "rv32_runner.h"

static inline uint32_t timer_read32(void) {
    return (uint32_t)_ecall0(RV32_ECALL_TIMER_READ32).a0;
}

static inline uint32_t g_rgb_timer(void) {
    return (uint32_t)_ecall0(RV32_ECALL_RGB_TIMER).a0;
}

static inline RV32_HSV rgb_matrix_config_hsv(void) {
    RV32_HSV hsv;
    *(uint32_t *)&hsv = _ecall0(RV32_ECALL_RGB_MATRIX_CONFIG_HSV).a0;
    return hsv;
}

static inline uint8_t rgb_matrix_config_speed(void) {
    return _ecall0(RV32_ECALL_RGB_MATRIX_CONFIG_SPEED).a0;
}

static inline void rgb_matrix_set_color(int index, uint8_t red, uint8_t green, uint8_t blue) {
    _ecall4(RV32_ECALL_RGB_MATRIX_SET_COLOR, index, red, green, blue);
}

static inline uint32_t rand() {
    return _ecall0(RV32_ECALL_RAND).a0;
}

static inline uint16_t scale16by8(uint16_t x, uint8_t y) {
    return _ecall2(RV32_ECALL_SCALE16BY8, x, y).a0;
}

static inline uint8_t scale8(uint8_t x, uint8_t y) {
    return _ecall2(RV32_ECALL_SCALE8, x, y).a0;
}

static inline uint8_t abs8(uint8_t x) {
    return _ecall1(RV32_ECALL_ABS8, x).a0;
}

static inline uint8_t sin8(uint8_t x) {
    return _ecall1(RV32_ECALL_SIN8, x).a0;
}

static inline RV32_RGB rgb_matrix_hsv_to_rgb(RV32_HSV hsv) {
    RV32_RGB rgb;
    *(uint32_t *)&rgb = _ecall1(RV32_ECALL_HSV_TO_RGB, *(uint32_t *)&hsv).a0;
    return rgb;
}

#define RGB_MATRIX_LED_COUNT 42

static uint16_t time_offsets[RGB_MATRIX_LED_COUNT] = {0};
RV32_HSV        hsv                                = {0};
uint8_t         speed                              = 0;

void effect_init(void) {
    hsv   = rgb_matrix_config_hsv();
    speed = rgb_matrix_config_speed();
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        time_offsets[i] = rand() % 65500;
    }
}

void effect_led(void *params, uint8_t led_index) {
    uint16_t time = scale16by8((g_rgb_timer() / 2) + time_offsets[led_index], speed / 16);
    hsv.v         = scale8(abs8(sin8(time) - 128) * 2, hsv.v);
    RV32_RGB rgb  = rgb_matrix_hsv_to_rgb(hsv);
    rgb_matrix_set_color(led_index, rgb.r, rgb.g, rgb.b);
}
