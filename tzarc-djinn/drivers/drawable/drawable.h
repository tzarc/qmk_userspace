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
// Drawable types
typedef enum { DRAWABLE_ROTATION_0, DRAWABLE_ROTATION_90, DRAWABLE_ROTATION_180, DRAWABLE_ROTATION_270 } drawable_rotation_t;

typedef struct drawable_driver_t drawable_driver_t;

///////////////////////////////////////////////////////////////
// Drawable driver definitions
typedef void (*drawable_init_func)(drawable_driver_t *driver, drawable_rotation_t rotation);
typedef void (*drawable_clear_func)(drawable_driver_t *driver);
typedef void (*drawable_power_func)(drawable_driver_t *driver, bool power_on);
typedef void (*drawable_viewport_func)(drawable_driver_t *driver, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
typedef void (*drawable_pixdata_func)(drawable_driver_t *driver, const uint16_t *pixel_data, uint32_t num_pixels);
typedef void (*drawable_setpixel_func)(drawable_driver_t *driver, uint16_t x, uint16_t y, uint16_t color);
typedef void (*drawable_line_func)(drawable_driver_t *driver, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
typedef void (*drawable_rect_func)(drawable_driver_t *driver, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint16_t color, bool filled);
struct drawable_driver_t {
    drawable_init_func     init;
    drawable_clear_func    clear;
    drawable_power_func    power;
    drawable_viewport_func viewport;
    drawable_pixdata_func  pixdata;
    drawable_setpixel_func setpixel;
    drawable_line_func     line;
    drawable_rect_func     rect;
};

///////////////////////////////////////////////////////////////
// Drawable API
void drawable_init(drawable_driver_t *driver, drawable_rotation_t rotation);
void drawable_clear(drawable_driver_t *driver);
void drawable_power(drawable_driver_t *driver, bool power_on);
void drawable_viewport(drawable_driver_t *driver, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
void drawable_pixdata(drawable_driver_t *driver, const uint16_t *pixel_data, uint32_t num_pixels);
void drawable_setpixel(drawable_driver_t *driver, uint16_t x, uint16_t y, uint16_t color);
void drawable_line(drawable_driver_t *driver, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void drawable_rect(drawable_driver_t *driver, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint16_t color, bool filled);