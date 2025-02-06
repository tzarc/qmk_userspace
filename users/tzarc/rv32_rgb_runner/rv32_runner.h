// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <stdint.h>

// List of rv32 register indices
enum {
    rv32reg_zero = 0,
    rv32reg_ra,
    rv32reg_sp,
    rv32reg_gp,
    rv32reg_tp,
    rv32reg_t0,
    rv32reg_t1,
    rv32reg_t2,
    rv32reg_s0,
    rv32reg_s1,
    rv32reg_a0,
    rv32reg_a1,
    rv32reg_a2,
    rv32reg_a3,
    rv32reg_a4,
    rv32reg_a5,
    rv32reg_a6,
    rv32reg_a7,
    rv32reg_s2,
    rv32reg_s3,
    rv32reg_s4,
    rv32reg_s5,
    rv32reg_s6,
    rv32reg_s7,
    rv32reg_s8,
    rv32reg_s9,
    rv32reg_s10,
    rv32reg_s11,
    rv32reg_t3,
    rv32reg_t4,
    rv32reg_t5,
    rv32reg_t6,
};

// This list needs to be kept in-sync with the api_table in rv32_runner.S
typedef enum rv32_api_t {
    RV32_EFFECT_INIT = 0,
    RV32_EFFECT_BEGIN_ITER,
    RV32_EFFECT_LED,
    RV32_EFFECT_END_ITER,
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
