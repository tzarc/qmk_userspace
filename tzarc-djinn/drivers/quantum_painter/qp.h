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

///////////////////////////////////////////////////////////////
// Quantum Painter types
typedef enum { QP_ROTATION_0, QP_ROTATION_90, QP_ROTATION_180, QP_ROTATION_270 } painter_rotation_t;
typedef void *painter_device_t;

///////////////////////////////////////////////////////////////
// Quantum Painter API
void qp_init(painter_device_t *device, painter_rotation_t rotation);
void qp_clear(painter_device_t *device);
void qp_power(painter_device_t *device, bool power_on);
void qp_viewport(painter_device_t *device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
void qp_pixdata(painter_device_t *device, const void *pixel_data, uint32_t num_pixels);
void qp_setpixel(painter_device_t *device, uint16_t x, uint16_t y, HSV color);
void qp_line(painter_device_t *device, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, HSV color);
void qp_rect(painter_device_t *device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, HSV color, bool filled);