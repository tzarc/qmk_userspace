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
typedef enum { IMAGE_FORMAT_RAW, IMAGE_FORMAT_RGB565, IMAGE_FORMAT_MONO4BPP, IMAGE_FORMAT_MONO2BPP, IMAGE_FORMAT_MONO1BPP } painter_image_format_t;
typedef void *painter_device_t;

///////////////////////////////////////////////////////////////
// Quantum Painter API
bool qp_init(painter_device_t device, painter_rotation_t rotation);
bool qp_clear(painter_device_t device);
bool qp_power(painter_device_t device, bool power_on);
bool qp_viewport(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
bool qp_pixdata(painter_device_t device, const void *pixel_data, uint32_t byte_count);
bool qp_setpixel(painter_device_t device, uint16_t x, uint16_t y, uint8_t hue, uint8_t sat, uint8_t val);
bool qp_line(painter_device_t device, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t hue, uint8_t sat, uint8_t val);
bool qp_rect(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint8_t hue, uint8_t sat, uint8_t val, bool filled);
bool qp_drawimage(painter_device_t device, uint16_t x, uint16_t y, uint16_t w, uint16_t h, painter_image_format_t format, const void *pixel_data, uint32_t byte_count);
