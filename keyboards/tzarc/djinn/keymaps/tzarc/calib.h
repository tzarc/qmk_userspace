// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <stddef.h>
#include <stdint.h>

// Import the normal SPI flash datatypes
#include "drivers/flash/flash_spi.h"

// Redefined APIs, implemented in calib.c
void calib_init(void);
flash_status_t calib_erase_chip(void);
flash_status_t calib_erase_block(uint32_t addr);
flash_status_t calib_erase_sector(uint32_t addr);
flash_status_t calib_read_block(uint32_t addr, void *buf, size_t len);
flash_status_t calib_write_block(uint32_t addr, const void *buf, size_t len);
