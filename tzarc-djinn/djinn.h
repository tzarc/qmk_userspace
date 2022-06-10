// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <quantum.h>
#include <qp.h>

//----------------------------------------------------------
// Layout

// clang-format off
#if 0
#define LAYOUT_djinn( \
        k00, k01, k02, k03, k04, k05, k06,                                k07, k08, k09, k0A, k0B, k0C, k0D, \
        k10, k11, k12, k13, k14, k15, k16,                                k17, k18, k19, k1A, k1B, k1C, k1D, \
        k20, k21, k22, k23, k24, k25, k26,                                k27, k28, k29, k2A, k2B, k2C, k2D, \
        k30, k31, k32, k33, k34, k35, k36,                                k37, k38, k39, k3A, k3B, k3C, k3D, \
                       k40, k41, k42, k43,                                k44, k45, k46, k47,                \
                                                k50,            k51,                                         \
                              k60,                                               k61,                        \
                         k70, k71, k72,                                     k73, k74, k75,                   \
                              k80,                                               k81                         \
    )                                                                                                        \
    {                                                                                                        \
    /*    0      1      2      3      4      5      6           */                                           \
        {k00,   k01,   k02,   k03,   k04,   k05,   k06},  /*  0 */                                           \
        {k10,   k11,   k12,   k13,   k14,   k15,   k16},  /*  1 */                                           \
        {k20,   k21,   k22,   k23,   k24,   k25,   k26},  /*  2 */                                           \
        {k30,   k31,   k32,   k33,   k34,   k35,   k36},  /*  3 */                                           \
        {KC_NO, KC_NO, KC_NO, k40,   k41,   k42,   k43},  /*  4 */                                           \
        {KC_NO, k60,   k70,   k80,   k72,   k71,   k50},  /*  5 */                                           \
        {k0D,   k0C,   k0B,   k0A,   k09,   k08,   k07},  /*  6 */                                           \
        {k1D,   k1C,   k1B,   k1A,   k19,   k18,   k17},  /*  7 */                                           \
        {k2D,   k2C,   k2B,   k2A,   k29,   k28,   k27},  /*  8 */                                           \
        {k3D,   k3C,   k3B,   k3A,   k39,   k38,   k37},  /*  9 */                                           \
        {KC_NO, KC_NO, KC_NO, k47,   k46,   k45,   k44},  /* 10 */                                           \
        {KC_NO, k61,   k73,   k81,   k75,   k74,   k51},  /* 11 */                                           \
    }
#endif
// clang-format on

//----------------------------------------------------------
// Runtime data sync -- keyboard

extern painter_device_t lcd;

#pragma pack(push)
#pragma pack(1)
typedef struct kb_runtime_config {
    unsigned          lcd_power : 1;
    usbpd_allowance_t current_setting : 2;
} kb_runtime_config;
#pragma pack(pop)

extern kb_runtime_config kb_state;

void matrix_wait_for_interrupt(void);

void kb_state_update(void);
void kb_state_sync(void);
void kb_state_sync_slave(uint8_t m2s_size, const void* m2s_buffer, uint8_t s2m_size, void* s2m_buffer);

void        usbpd_update(void);
const char* usbpd_str(usbpd_allowance_t allowance);
