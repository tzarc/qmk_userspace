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

///////////////////////////////////////////////////////////////
// Quantum Painter definitions
typedef void (*painter_driver_init_func)(painter_device_t *driver, painter_rotation_t rotation);
typedef void (*painter_driver_clear_func)(painter_device_t *driver);
typedef void (*painter_driver_power_func)(painter_device_t *driver, bool power_on);
typedef void (*painter_driver_viewport_func)(painter_device_t *driver, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
typedef void (*painter_driver_pixdata_func)(painter_device_t *driver, const void *pixel_data, uint32_t num_pixels);
typedef void (*painter_driver_setpixel_func)(painter_device_t *driver, uint16_t x, uint16_t y, HSV color);
typedef void (*painter_driver_line_func)(painter_device_t *driver, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, HSV color);
typedef void (*painter_driver_rect_func)(painter_device_t *driver, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, HSV color, bool filled);

struct painter_driver_t {
    painter_driver_init_func     init;
    painter_driver_clear_func    clear;
    painter_driver_power_func    power;
    painter_driver_viewport_func viewport;
    painter_driver_pixdata_func  pixdata;
    painter_driver_setpixel_func setpixel;
    painter_driver_line_func     line;
    painter_driver_rect_func     rect;
};
