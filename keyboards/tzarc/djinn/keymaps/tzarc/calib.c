// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "calib.h"

// Redefine all SPI flash APIs so we can use a different setup
#define flash_init calib_init
#define flash_erase_chip calib_erase_chip
#define flash_erase_sector calib_erase_sector
#define flash_erase_block calib_erase_block
#define flash_read_block calib_read_block
#define flash_write_block calib_write_block
#include "drivers/flash/flash_spi.c"
