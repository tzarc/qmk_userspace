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

#include "qp.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Quantum Painter ILI9341 configurables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The number of ILI9341 devices we're going to be talking to
#ifndef ILI9341_NUM_DEVICES
#    define ILI9341_NUM_DEVICES 1
#endif

// The SPI clock divisor to use when talking to the LCD -- will need to take into account your MCUs speed and the maximum allowed speed of the LCD
#ifndef ILI9341_SPI_DIVISOR
#    define ILI9341_SPI_DIVISOR 8
#endif

// The buffer size to use when rendering chunks of data, allows limiting of RAM allocation when rendering images
#ifndef ILI9341_PIXDATA_BUFSIZE
#    define ILI9341_PIXDATA_BUFSIZE 32
#endif

#if ILI9341_PIXDATA_BUFSIZE < 16
#    error ILI9341 pixel buffer size too small -- ILI9341_PIXDATA_BUFSIZE must be >= 16
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Quantum Painter ILI9341 device factory
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

painter_device_t qp_ili9341_make_device(pin_t chip_select_pin, pin_t data_pin, pin_t reset_pin, bool uses_backlight);
