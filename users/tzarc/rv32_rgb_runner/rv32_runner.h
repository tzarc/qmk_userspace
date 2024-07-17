// Copyright 2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// This list needs to be kept in-sync with the api_table in rv32_runner.S
typedef enum rv32_api_t {
    RV32_EFFECT_INIT = 0,
    RV32_EFFECT_LED,
} rv32_api_t;

// This list needs to be kept in sync with rv32_runner.c
typedef enum rv32_calls_t {
    RV32_VCALL_TIMER_READ32,
    RV32_VCALL_RGB_MATRIX_SET_COLOR,
} rv32_calls_t;

typedef enum rv32_code_t {
    RV32_EXIT     = 0x5555,
    RV32_VCALL    = 0x9999,
} rv32_code_t;
