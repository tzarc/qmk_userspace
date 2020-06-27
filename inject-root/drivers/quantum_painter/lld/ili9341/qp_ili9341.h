/* Copyright 2018-2020 Nick Brassel (@tzarc)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include "qp_common.h"

#ifndef ILI9341_NUM_DEVICES
#    define ILI9341_NUM_DEVICES 1
#endif

#ifndef ILI9341_SPI_DIVISOR
#    define ILI9341_SPI_DIVISOR 8
#endif

#ifndef ILI9341_PIXDATA_BUFSIZE
#    define ILI9341_PIXDATA_BUFSIZE 32
#endif

painter_device_t *qp_make_ili9341_device(pin_t chip_select_pin, pin_t data_pin, pin_t reset_pin, bool uses_backlight);
