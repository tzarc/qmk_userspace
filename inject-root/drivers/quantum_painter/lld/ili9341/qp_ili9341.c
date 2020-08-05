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

#include <string.h>
#include <color.h>
#include <spi_master.h>

#include "qp_internal.h"
#include "qp_utils.h"
#include "qp_fallback.h"
#include "qp_ili9341.h"
#include "qp_ili9341_opcodes.h"

#define BYTE_SWAP(x) (((x >> 8) & 0x00FF) | ((x << 8) & 0xFF00))

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Device definition
typedef struct ili9341_painter_device_t {
    struct painter_driver_t qp_driver;  // must be first, so it can be cast from the painter_device_t* type
    bool                    allocated;
    pin_t                   chip_select_pin;
    pin_t                   data_pin;
    pin_t                   reset_pin;
    painter_rotation_t      rotation;
    bool                    uses_backlight;
} ili9341_painter_device_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool qp_ili9341_init(painter_device_t device, painter_rotation_t rotation);
bool qp_ili9341_clear(painter_device_t device);
bool qp_ili9341_power(painter_device_t device, bool power_on);
bool qp_ili9341_viewport(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
bool qp_ili9341_pixdata(painter_device_t device, const void *pixel_data, uint32_t byte_count);
bool qp_ili9341_setpixel(painter_device_t device, uint16_t x, uint16_t y, uint8_t hue, uint8_t sat, uint8_t val);
bool qp_ili9341_line(painter_device_t device, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t hue, uint8_t sat, uint8_t val);
bool qp_ili9341_rect(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint8_t hue, uint8_t sat, uint8_t val, bool filled);
bool qp_ili9341_drawimage(painter_device_t device, uint16_t x, uint16_t y, const painter_image_descriptor_t *image);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Low-level LCD control functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Enable SPI comms
static inline void lcd_start(ili9341_painter_device_t *lcd) { spi_start(lcd->chip_select_pin, false, 0, ILI9341_SPI_DIVISOR); }

// Disable SPI comms
static inline void lcd_stop(void) { spi_stop(); }

// Send a command
static inline void lcd_cmd(ili9341_painter_device_t *lcd, uint8_t b) {
    writePinLow(lcd->data_pin);
    spi_write(b);
}

// Send data
static inline void lcd_sendbuf(ili9341_painter_device_t *lcd, const void *data, uint16_t len) {
    writePinHigh(lcd->data_pin);
    spi_transmit(data, len);
}

// Send data (single byte)
static inline void lcd_data(ili9341_painter_device_t *lcd, uint8_t b) { lcd_sendbuf(lcd, &b, sizeof(b)); }

// Set a register value
static inline void lcd_reg(ili9341_painter_device_t *lcd, uint8_t reg, uint8_t val) {
    lcd_cmd(lcd, reg);
    lcd_data(lcd, val);
}

// Set the drawing viewport position
static inline void lcd_viewport(ili9341_painter_device_t *lcd, uint16_t xbegin, uint16_t ybegin, uint16_t xend, uint16_t yend) {
    // Set up the x-window
    uint8_t xbuf[4] = {xbegin >> 8, xbegin & 0xFF, xend >> 8, xend & 0xFF};
    lcd_cmd(lcd, 0x2A);  // column address set
    lcd_sendbuf(lcd, xbuf, sizeof(xbuf));

    // Set up the y-window
    uint8_t ybuf[4] = {ybegin >> 8, ybegin & 0xFF, yend >> 8, yend & 0xFF};
    lcd_cmd(lcd, 0x2B);  // page (row) address set
    lcd_sendbuf(lcd, ybuf, sizeof(ybuf));

    // Lock in the window
    lcd_cmd(lcd, 0x2C);  // memory write
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Colour conversion to LCD-native
static inline uint16_t rgb_to_ili9341(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t rgb565 = (((uint16_t)r) >> 3) << 11 | (((uint16_t)g) >> 2) << 5 | (((uint16_t)b) >> 3);
    return BYTE_SWAP(rgb565);
}

// Colour conversion to LCD-native
static inline uint16_t hsv_to_ili9341(uint8_t hue, uint8_t sat, uint8_t val) {
    RGB rgb = hsv_to_rgb_nocie((HSV){hue, sat, val});
    return rgb_to_ili9341(rgb.r, rgb.g, rgb.b);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Palette / Monochrome-format image rendering
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Palette renderer
static inline void lcd_send_palette_pixdata_impl(ili9341_painter_device_t *lcd, const uint16_t *const rgb565_palette, uint8_t bits_per_pixel, uint32_t pixel_count, const void *const pixel_data, uint32_t byte_count) {
    uint16_t       buf[ILI9341_PIXDATA_BUFSIZE];
    const uint8_t  pixel_bitmask       = (1 << bits_per_pixel) - 1;
    const uint8_t  pixels_per_byte     = 8 / bits_per_pixel;
    const uint16_t max_transmit_pixels = ((ILI9341_PIXDATA_BUFSIZE / pixels_per_byte) * pixels_per_byte);  // the number of rgb565 pixels that we can complete fit in the buffer
    const uint8_t *pixdata             = (const uint8_t *)pixel_data;
    uint32_t       remaining_pixels    = pixel_count;  // don't try to derive from byte_count, we may not use an entire byte

    // Transmit each block of pixels
    while (remaining_pixels > 0) {
        uint16_t  transmit_pixels = remaining_pixels < max_transmit_pixels ? remaining_pixels : max_transmit_pixels;
        uint16_t *target16        = (uint16_t *)buf;
        for (uint16_t p = 0; p < transmit_pixels; p += pixels_per_byte) {
            uint8_t pixval      = *pixdata;
            uint8_t loop_pixels = remaining_pixels < pixels_per_byte ? remaining_pixels : pixels_per_byte;
            for (uint8_t q = 0; q < loop_pixels; ++q) {
                *target16++ = rgb565_palette[pixval & pixel_bitmask];
                pixval >>= bits_per_pixel;
            }
            ++pixdata;
        }
        lcd_sendbuf(lcd, buf, transmit_pixels * sizeof(uint16_t));
        remaining_pixels -= transmit_pixels;
    }
}

// Recoloured renderer
static inline void lcd_send_palette_pixdata(ili9341_painter_device_t *lcd, const uint8_t *const rgb_palette, uint8_t bits_per_pixel, uint32_t pixel_count, const void *const pixel_data, uint32_t byte_count) {
    // Generate the colour lookup table
    uint16_t rgb565_palette[256];
    uint16_t items = 1 << bits_per_pixel;  // number of items we need to intepolate
    for (uint16_t i = 0; i < items; ++i) {
        rgb565_palette[i] = rgb_to_ili9341(rgb_palette[i * 3 + 0], rgb_palette[i * 3 + 1], rgb_palette[i * 3 + 2]);
    }

    // Transmit each block of pixels
    lcd_send_palette_pixdata_impl(lcd, rgb565_palette, bits_per_pixel, pixel_count, pixel_data, byte_count);
}

// Recoloured renderer
static inline void lcd_send_mono_pixdata_recolour(ili9341_painter_device_t *lcd, uint8_t bits_per_pixel, uint32_t pixel_count, const void *const pixel_data, uint32_t byte_count, int16_t hue_fg, int16_t sat_fg, int16_t val_fg, int16_t hue_bg, int16_t sat_bg, int16_t val_bg) {
    // Generate the colour lookup table
    HSV      hsv_lookup_table[16];
    uint16_t rgb565_palette[16];
    uint8_t  items = 1 << bits_per_pixel;  // number of items we need to intepolate
    qp_generate_palette(hsv_lookup_table, items, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
    for (uint8_t i = 0; i < items; ++i) {
        rgb565_palette[i] = hsv_to_ili9341(hsv_lookup_table[i].h, hsv_lookup_table[i].s, hsv_lookup_table[i].v);
    }

    // Transmit each block of pixels
    lcd_send_palette_pixdata_impl(lcd, rgb565_palette, bits_per_pixel, pixel_count, pixel_data, byte_count);
}

// Default implementation is greyscale
static inline void lcd_send_mono_pixdata(ili9341_painter_device_t *lcd, uint8_t bits_per_pixel, uint32_t pixel_count, const void *const pixel_data, uint32_t byte_count) { lcd_send_mono_pixdata_recolour(lcd, bits_per_pixel, pixel_count, pixel_data, byte_count, 0, 0, 255, 0, 0, 0); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Quantum Painter API implementations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Initialisation
bool qp_ili9341_init(painter_device_t device, painter_rotation_t rotation) {
    static const uint8_t pgamma[15] = {0x0F, 0x29, 0x24, 0x0C, 0x0E, 0x09, 0x4E, 0x78, 0x3C, 0x09, 0x13, 0x05, 0x17, 0x11, 0x00};
    static const uint8_t ngamma[15] = {0x00, 0x16, 0x1B, 0x04, 0x11, 0x07, 0x31, 0x33, 0x42, 0x05, 0x0C, 0x0A, 0x28, 0x2F, 0x0F};

    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;
    lcd->rotation                 = rotation;

    // Initialise the SPI peripheral
    spi_init();

    // Set up pin directions and initial values
    setPinOutput(lcd->chip_select_pin);
    writePinHigh(lcd->chip_select_pin);

    setPinOutput(lcd->data_pin);
    writePinLow(lcd->data_pin);

    setPinOutput(lcd->reset_pin);

    // Perform a reset
    writePinLow(lcd->reset_pin);
    wait_ms(20);
    writePinHigh(lcd->reset_pin);
    wait_ms(20);

    // Ebable the SPI comms to the LCS
    lcd_start(lcd);

    // Configure power control
    lcd_cmd(lcd, ILI9341_POWER_CTL_A);
    lcd_data(lcd, 0x39);
    lcd_data(lcd, 0x2C);
    lcd_data(lcd, 0x00);
    lcd_data(lcd, 0x34);
    lcd_data(lcd, 0x02);

    lcd_cmd(lcd, ILI9341_POWER_CTL_B);
    lcd_data(lcd, 0x00);
    lcd_data(lcd, 0xD9);
    lcd_data(lcd, 0X30);

    lcd_cmd(lcd, ILI9341_POWER_ON_SEQ_CTL);
    lcd_data(lcd, 0x64);
    lcd_data(lcd, 0x03);
    lcd_data(lcd, 0X12);
    lcd_data(lcd, 0X81);

    lcd_cmd(lcd, ILI9341_SET_PUMP_RATIO_CTL);
    lcd_data(lcd, 0x20);

    lcd_cmd(lcd, ILI9341_SET_POWER_CTL_1);
    lcd_data(lcd, 0x26);

    lcd_cmd(lcd, ILI9341_SET_POWER_CTL_2);
    lcd_data(lcd, 0x11);

    lcd_cmd(lcd, ILI9341_SET_VCOM_CTL_1);
    lcd_data(lcd, 0x35);
    lcd_data(lcd, 0x3E);

    lcd_cmd(lcd, ILI9341_SET_VCOM_CTL_2);
    lcd_data(lcd, 0xBE);

    // Configure timing control
    lcd_cmd(lcd, ILI9341_DRV_TIMING_CTL_A);
    lcd_data(lcd, 0x85);
    lcd_data(lcd, 0x10);
    lcd_data(lcd, 0x7A);

    lcd_cmd(lcd, ILI9341_DRV_TIMING_CTL_B);
    lcd_data(lcd, 0x00);
    lcd_data(lcd, 0x00);

    // Configure brightness / gamma
    lcd_cmd(lcd, ILI9341_SET_BRIGHTNESS);
    lcd_data(lcd, 0xFF);

    lcd_cmd(lcd, ILI9341_ENABLE_3_GAMMA);
    lcd_data(lcd, 0x00);

    lcd_cmd(lcd, ILI9341_SET_GAMMA);
    lcd_data(lcd, 0x01);

    lcd_cmd(lcd, ILI9341_SET_PGAMMA);
    lcd_sendbuf(lcd, pgamma, sizeof(pgamma));

    lcd_cmd(lcd, ILI9341_SET_NGAMMA);
    lcd_sendbuf(lcd, ngamma, sizeof(ngamma));

    // Set the pixel format
    lcd_cmd(lcd, ILI9341_SET_PIX_FMT);
    lcd_data(lcd, 0x55);

    lcd_cmd(lcd, ILI9341_SET_FRAME_CTL_NORMAL);
    lcd_data(lcd, 0x00);
    lcd_data(lcd, 0x1B);

    lcd_cmd(lcd, ILI9341_SET_FUNCTION_CTL);
    lcd_data(lcd, 0x0A);
    lcd_data(lcd, 0xA2);

    // Set the default viewport to be fullscreen
    lcd_viewport(lcd, 0, 0, 239, 319);

    // Configure the rotation (i.e. the ordering and direction of memory writes in GRAM)
    switch (rotation) {
        case QP_ROTATION_0:
            lcd_cmd(lcd, ILI9341_SET_MEM_ACS_CTL);
            lcd_data(lcd, 0b00001000);
            break;
        case QP_ROTATION_90:
            lcd_cmd(lcd, ILI9341_SET_MEM_ACS_CTL);
            lcd_data(lcd, 0b10101000);
            break;
        case QP_ROTATION_180:
            lcd_cmd(lcd, ILI9341_SET_MEM_ACS_CTL);
            lcd_data(lcd, 0b11001000);
            break;
        case QP_ROTATION_270:
            lcd_cmd(lcd, ILI9341_SET_MEM_ACS_CTL);
            lcd_data(lcd, 0b01101000);
            break;
    }

    // Disable sleep mode
    lcd_cmd(lcd, ILI9341_CMD_SLEEP_OFF);
    wait_ms(20);

    // Disable the SPI comms to the LCS
    lcd_stop();

    return true;
}

// Screen clear
bool qp_ili9341_clear(painter_device_t device) {
    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;

    // Re-init the LCD
    qp_ili9341_init(device, lcd->rotation);

    return true;
}

// Power control -- on/off (will handle backlight if using normal QMK backlight driver)
bool qp_ili9341_power(painter_device_t device, bool power_on) {
    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;
    lcd_start(lcd);

    // Turn on/off the display
    lcd_cmd(lcd, power_on ? ILI9341_CMD_DISPLAY_ON : ILI9341_CMD_DISPLAY_OFF);

    lcd_stop();

#ifdef BACKLIGHT_PIN
    // If we're using the backlight to control the display as well, toggle that too.
    if (lcd->uses_backlight) {
        if (power_on) {
            // There's a small amount of time for the LCD to get the display back on the screen -- it's all white beforehand.
            // Delay for a small amount of time and let the LCD catch up before turning the backlight on.
            wait_ms(20);
            backlight_enable();
        } else
            backlight_disable();
    }
#endif

    return true;
}

// Viewport to draw to
bool qp_ili9341_viewport(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) {
    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;
    lcd_start(lcd);

    // Configure where we're going to be rendering to
    lcd_viewport(lcd, left, top, right, bottom);

    lcd_stop();

    return true;
}

// Stream pixel data to the current write position in GRAM
bool qp_ili9341_pixdata(painter_device_t device, const void *pixel_data, uint32_t byte_count) {
    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;
    lcd_start(lcd);

    // Stream data to the LCD
    lcd_sendbuf(lcd, pixel_data, byte_count);

    lcd_stop();

    return true;
}

// Manually set a single pixel's colour
bool qp_ili9341_setpixel(painter_device_t device, uint16_t x, uint16_t y, uint8_t hue, uint8_t sat, uint8_t val) {
    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;
    lcd_start(lcd);

    // Configure where we're going to be rendering to
    lcd_viewport(lcd, x, y, x, y);

    // Convert the color to RGB565 and transmit to the device
    uint16_t buf = hsv_to_ili9341(hue, sat, val);
    lcd_sendbuf(lcd, &buf, sizeof(buf));

    lcd_stop();

    return true;
}

// Draw a line
bool qp_ili9341_line(painter_device_t device, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t hue, uint8_t sat, uint8_t val) {
    // If we're not doing horizontal or vertical, fallback to the base implementation
    if (x0 != x1 && y0 != y1) {
        return qp_fallback_line(device, x0, y0, x1, y1, hue, sat, val);
    }

    // If we're doing horizontal or vertical, just use the optimised rect draw so we don't need to deal with single pixels or buffers.
    return qp_ili9341_rect(device, x0, y0, x1, y1, hue, sat, val, true);
}

// Draw a rectangle
bool qp_ili9341_rect(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint8_t hue, uint8_t sat, uint8_t val, bool filled) {
    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;

    if (filled) {
        // Convert the color to RGB565
        uint16_t clr = hsv_to_ili9341(hue, sat, val);

        // Build a larger buffer so we can stream to the LCD in larger chunks, for speed
        uint16_t buf[ILI9341_PIXDATA_BUFSIZE];
        for (uint32_t i = 0; i < ILI9341_PIXDATA_BUFSIZE; ++i) buf[i] = clr;

        lcd_start(lcd);

        // Configure where we're going to be rendering to
        lcd_viewport(lcd, left, top, right, bottom);

        // Transmit the data to the LCD in chunks
        uint32_t remaining = (right - left + 1) * (bottom - top + 1);
        while (remaining > 0) {
            uint32_t transmit = (remaining < ILI9341_PIXDATA_BUFSIZE ? remaining : ILI9341_PIXDATA_BUFSIZE);
            uint32_t bytes    = transmit * sizeof(uint16_t);
            lcd_sendbuf(lcd, buf, bytes);
            remaining -= transmit;
        }

        lcd_stop();
    } else {
        if (!qp_ili9341_rect(device, left, top, right, top, hue, sat, val, true)) {
            return false;
        }
        if (!qp_ili9341_rect(device, left, bottom, right, bottom, hue, sat, val, true)) {
            return false;
        }
        if (!qp_ili9341_rect(device, left, top + 1, left, bottom - 1, hue, sat, val, true)) {
            return false;
        }
        if (!qp_ili9341_rect(device, right, top + 1, right, bottom - 1, hue, sat, val, true)) {
            return false;
        }
    }

    return true;
}

// Draw an image
bool qp_ili9341_drawimage(painter_device_t device, uint16_t x, uint16_t y, const painter_image_descriptor_t *image) {
    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;
    lcd_start(lcd);

    // Configure where we're going to be rendering to
    lcd_viewport(lcd, x, y, x + image->width - 1, y + image->height - 1);

    uint32_t pixel_count = (((uint32_t)image->width) * image->height);
    if (image->compression == IMAGE_COMPRESSED_LZF) {
        const painter_compressed_image_descriptor_t *comp_image_desc = (const painter_compressed_image_descriptor_t *)image;
        uint8_t                                      buf[QUANTUM_PAINTER_COMPRESSED_CHUNK_SIZE];
        for (uint16_t i = 0; i < comp_image_desc->chunk_count; ++i) {
            // Check if we're the last chunk
            bool last_chunk = (i == (comp_image_desc->chunk_count - 1));
            // Work out the current chunk size
            uint32_t compressed_size = last_chunk ? (comp_image_desc->compressed_size - comp_image_desc->chunk_offsets[i])        // last chunk
                                                  : (comp_image_desc->chunk_offsets[i + 1] - comp_image_desc->chunk_offsets[i]);  // any other chunk
            // Decode the image data
            uint32_t decompressed_size = qp_decode(&comp_image_desc->compressed_data[comp_image_desc->chunk_offsets[i]], compressed_size, buf, sizeof(buf));

            // Stream data to the LCD
            if (image->image_format == IMAGE_FORMAT_RAW || image->image_format == IMAGE_FORMAT_RGB565) {
                // The pixel data is in the correct format already -- send it directly to the device
                lcd_sendbuf(lcd, buf, decompressed_size);
                pixel_count -= decompressed_size / 2;
            } else if (image->image_format == IMAGE_FORMAT_GREYSCALE) {
                uint32_t pixels_this_loop = last_chunk ? pixel_count : (comp_image_desc->chunk_size * 8 / comp_image_desc->image_bpp);
                lcd_send_mono_pixdata(lcd, comp_image_desc->image_bpp, pixels_this_loop, buf, decompressed_size);
                pixel_count -= pixels_this_loop;
            } else if (image->image_format == IMAGE_FORMAT_PALETTE) {
                uint32_t pixels_this_loop = last_chunk ? pixel_count : (comp_image_desc->chunk_size * 8 / comp_image_desc->image_bpp);
                lcd_send_palette_pixdata(lcd, comp_image_desc->image_palette, comp_image_desc->image_bpp, pixels_this_loop, buf, decompressed_size);
                pixel_count -= pixels_this_loop;
            }
        }
    } else if (image->compression == IMAGE_UNCOMPRESSED) {
        const painter_raw_image_descriptor_t *raw_image_desc = (const painter_raw_image_descriptor_t *)image;
        // Stream data to the LCD
        if (image->image_format == IMAGE_FORMAT_RAW || image->image_format == IMAGE_FORMAT_RGB565) {
            uint32_t bytes_remaining = raw_image_desc->byte_count;
            const uint8_t* data = raw_image_desc->image_data;
            while(bytes_remaining > 0) {
                uint32_t bytes_this_loop = bytes_remaining < 1024 ? bytes_remaining : 1024;
                // The pixel data is in the correct format already -- send it directly to the device
                lcd_sendbuf(lcd, data, bytes_this_loop);
                data += bytes_this_loop;
                bytes_remaining -= bytes_this_loop;
            }
        } else if (image->image_format == IMAGE_FORMAT_GREYSCALE) {
            // Supplied pixel data is in 4bpp monochrome -- decode it to the equivalent pixel data
            lcd_send_mono_pixdata(lcd, raw_image_desc->image_bpp, pixel_count, raw_image_desc->image_data, raw_image_desc->byte_count);
        } else if (image->image_format == IMAGE_FORMAT_PALETTE) {
            // Supplied pixel data is in 1bpp monochrome -- decode it to the equivalent pixel data
            lcd_send_palette_pixdata(lcd, raw_image_desc->image_palette, raw_image_desc->image_bpp, pixel_count, raw_image_desc->image_data, raw_image_desc->byte_count);
        }
    }

    lcd_stop();

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Device creation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Driver storage
ili9341_painter_device_t drivers[ILI9341_NUM_DEVICES] = {0};

// Factory function for creating a handle to the ILI9341 device
painter_device_t qp_ili9341_make_device(pin_t chip_select_pin, pin_t data_pin, pin_t reset_pin, bool uses_backlight) {
    for (uint32_t i = 0; i < ILI9341_NUM_DEVICES; ++i) {
        ili9341_painter_device_t *driver = &drivers[i];
        memset(driver, 0, sizeof(ili9341_painter_device_t));
        if (!driver->allocated) {
            driver->allocated           = true;
            driver->qp_driver.init      = qp_ili9341_init;
            driver->qp_driver.clear     = qp_ili9341_clear;
            driver->qp_driver.power     = qp_ili9341_power;
            driver->qp_driver.pixdata   = qp_ili9341_pixdata;
            driver->qp_driver.viewport  = qp_ili9341_viewport;
            driver->qp_driver.setpixel  = qp_ili9341_setpixel;
            driver->qp_driver.line      = qp_ili9341_line;
            driver->qp_driver.rect      = qp_ili9341_rect;
            driver->qp_driver.drawimage = qp_ili9341_drawimage;
            driver->chip_select_pin     = chip_select_pin;
            driver->data_pin            = data_pin;
            driver->reset_pin           = reset_pin;
            driver->uses_backlight      = uses_backlight;
            return (painter_device_t)driver;
        }
    }
    return NULL;
}
