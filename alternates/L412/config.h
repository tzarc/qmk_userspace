/* Copyright 2018-2020 Nick Brassel (@tzarc)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

/* I2C parameters -> 1MHz I2C @ 48MHz clock */
#define I2C_DRIVER I2CD1
#define I2C1_BANK GPIOA
#define I2C1_SCL 9
#define I2C1_SDA 10
#define I2C1_SCL_PAL_MODE 4
#define I2C1_SDA_PAL_MODE 4
#define I2C1_TIMINGR_PRESC 0x00U
#define I2C1_TIMINGR_SCLDEL 0x03U
#define I2C1_TIMINGR_SDADEL 0x00U
#define I2C1_TIMINGR_SCLH 0x0FU
#define I2C1_TIMINGR_SCLL 0x33U

#include "common_config.h"
