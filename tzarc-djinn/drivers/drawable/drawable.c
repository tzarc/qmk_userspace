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

#include <quantum.h>
#include "drawable.h"

void drawable_init(drawable_driver_t *driver, drawable_rotation_t rotation) { driver->init(driver, rotation); }
void drawable_clear(drawable_driver_t *driver) { driver->clear(driver); }
void drawable_power(drawable_driver_t *driver, bool power_on) { driver->power(driver, power_on); }
void drawable_viewport(drawable_driver_t *driver, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) { driver->viewport(driver, left, top, right, bottom); }
void drawable_pixdata(drawable_driver_t *driver, const uint16_t *pixel_data, uint32_t num_pixels) { driver->pixdata(driver, pixel_data, num_pixels); }
void drawable_setpixel(drawable_driver_t *driver, uint16_t x, uint16_t y, uint16_t color) { driver->setpixel(driver, x, y, color); }
void drawable_line(drawable_driver_t *driver, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) { driver->line(driver, x0, y0, x1, y1, color); }
void drawable_rect(drawable_driver_t *driver, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint16_t color, bool filled) { driver->rect(driver, left, top, right, bottom, color, filled); }