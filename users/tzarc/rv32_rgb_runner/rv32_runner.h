// Copyright 2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <stdint.h>

// This list needs to be kept in-sync with the api_table in rv32_runner.S
typedef enum rv32_api_t {
    RV32_EFFECT_INIT = 0,
    RV32_EFFECT_LED,
} rv32_api_t;

// This list needs to be kept in sync with rv32_runner.c
typedef enum rv32_calls_t {
    RV32_EXIT = 0,
    RV32_ECALL_TIMER_READ32,
    RV32_ECALL_RGB_TIMER,
    RV32_ECALL_RGB_MATRIX_CONFIG_HSV,
    RV32_ECALL_RGB_MATRIX_CONFIG_SPEED,
    RV32_ECALL_RAND,
    RV32_ECALL_SCALE16BY8,
    RV32_ECALL_SCALE8,
    RV32_ECALL_ABS8,
    RV32_ECALL_SIN8,
    RV32_ECALL_HSV_TO_RGB,
    RV32_ECALL_RGB_MATRIX_SET_COLOR,
} rv32_calls_t;

typedef struct __attribute__((packed)) RV32_HSV {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} RV32_HSV;

typedef struct __attribute__((packed)) RV32_RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RV32_RGB;
