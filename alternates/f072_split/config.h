// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "config_common.h"

// #define EARLY_INIT_PERFORM_BOOTLOADER_JUMP TRUE

#define MATRIX_ROW_PINS \
    { B5 }
#define MATRIX_COL_PINS \
    { B4 }
#define UNUSED_PINS

#define SOFT_SERIAL_PIN A9
#define SERIAL_USART_TX_PAL_MODE 1
#define SERIAL_USART_SPEED 460800 // 9600 // 38400 // 115200 // 230400 // 460800 // 921600 // 1843200 // 2000000

#define SPLIT_USB_DETECT
#define SPLIT_HAND_PIN A2

#define DEBUG_MATRIX_SCAN_RATE
