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
#include <color.h>
#include "qp_common.h"

bool qp_init(painter_device_t device, painter_rotation_t rotation) {
    struct painter_driver_t *driver = (struct painter_driver_t *)device;
    return DRIVER_SUCCESS == driver->init(device, rotation);
}

bool qp_clear(painter_device_t device) {
    struct painter_driver_t *driver = (struct painter_driver_t *)device;
    return DRIVER_SUCCESS == driver->clear(device);
}

bool qp_power(painter_device_t device, bool power_on) {
    struct painter_driver_t *driver = (struct painter_driver_t *)device;
    return DRIVER_SUCCESS == driver->power(device, power_on);
}

bool qp_viewport(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) {
    struct painter_driver_t *driver = (struct painter_driver_t *)device;
    return DRIVER_SUCCESS == driver->viewport(device, left, top, right, bottom);
}

bool qp_pixdata(painter_device_t device, const void *pixel_data, uint32_t byte_count) {
    struct painter_driver_t *driver = (struct painter_driver_t *)device;
    return DRIVER_SUCCESS == driver->pixdata(device, pixel_data, byte_count);
}

bool qp_setpixel(painter_device_t device, uint16_t x, uint16_t y, uint8_t hue, uint8_t sat, uint8_t val) {
    struct painter_driver_t *driver = (struct painter_driver_t *)device;
    return DRIVER_SUCCESS == driver->setpixel(device, x, y, hue, sat, val);
}

bool qp_line(painter_device_t device, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t hue, uint8_t sat, uint8_t val) {
    // If the driver has an optimised implementation of line drawing, offload to the driver
    struct painter_driver_t *driver = (struct painter_driver_t *)device;
    if (driver->line) {
        painter_lld_status_t status = driver->line(device, x0, y0, x1, y1, hue, sat, val);
        switch (status) {
            case DRIVER_SUCCESS:
                return true;
            case DRIVER_FAILED:
                return false;
            case DRIVER_UNSUPPORTED:
                break;
        }
    }

    // Driver doesn't have an implementation -- fallback to setting pixels
    if (x0 == x1) {
        for (uint16_t y = y0; y <= y1; ++y) {
            if (DRIVER_SUCCESS != qp_setpixel(device, x0, y, hue, sat, val)) return false;
        }
    } else if (y0 == y1) {
        for (uint16_t x = x0; x <= x1; ++x) {
            if (DRIVER_SUCCESS != qp_setpixel(device, x, y0, hue, sat, val)) return false;
        }
    }

    return true;
}

bool qp_rect(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint8_t hue, uint8_t sat, uint8_t val, bool filled) {
    // If the driver has an optimised implementation of line drawing, offload to the driver
    struct painter_driver_t *driver = (struct painter_driver_t *)device;
    if (driver->rect) {
        painter_lld_status_t status = driver->rect(device, left, top, right, bottom, hue, sat, val, filled);
        switch (status) {
            case DRIVER_SUCCESS:
                return true;
            case DRIVER_FAILED:
                return false;
            case DRIVER_UNSUPPORTED:
                break;
        }
    }

    // Driver doesn't have an implementation -- fallback to drawing lines
    if (filled) {
        for (uint16_t y = top; y <= bottom; ++y) {
            if (DRIVER_SUCCESS != qp_line(device, left, y, right, y, hue, sat, val)) return false;
        }
    } else {
        if (DRIVER_SUCCESS != qp_line(device, left, top, right, top, hue, sat, val)) return false;
        if (DRIVER_SUCCESS != qp_line(device, left, bottom, right, bottom, hue, sat, val)) return false;
        if (DRIVER_SUCCESS != qp_line(device, left, top + 1, left, bottom - 1, hue, sat, val)) return false;
        if (DRIVER_SUCCESS != qp_line(device, right, top + 1, right, bottom - 1, hue, sat, val)) return false;
    }

    return true;
}

bool qp_drawimage(painter_device_t device, uint16_t x, uint16_t y, uint16_t w, uint16_t h, painter_image_format_t format, const void *pixel_data, uint32_t byte_count) {
    // If the driver has an optimised implementation of line drawing, offload to the driver
    struct painter_driver_t *driver = (struct painter_driver_t *)device;
    if (driver->drawimage) {
        painter_lld_status_t status = driver->drawimage(device, x, y, w, h, format, pixel_data, byte_count);
        switch (status) {
            case DRIVER_SUCCESS:
                return true;
            case DRIVER_FAILED:
                return false;
            case DRIVER_UNSUPPORTED:
                break;
        }
    }

    // If the driver doesn't want to handle it... we have no idea what format the display expects, so assume that the input data is native to the display.
    if (DRIVER_SUCCESS != qp_viewport(device, x, y, x + w - 1, y + h - 1)) return false;
    if (DRIVER_SUCCESS != qp_pixdata(device, pixel_data, byte_count)) return false;
    return true;
}