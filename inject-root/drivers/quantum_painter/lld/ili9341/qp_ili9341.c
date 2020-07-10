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
#include <qp_utils.h>
#include "qp_ili9341.h"

#if ILI9341_PIXDATA_BUFSIZE < 16
#    error ILI9341 pixel buffer size too small -- ILI9341_PIXDATA_BUFSIZE must be >= 16
#endif

#define BYTE_SWAP(x) (((x >> 8) & 0x00FF) | ((x << 8) & 0xFF00))

#define ILI9341_CMD_NOP 0x00                 // No operation
#define ILI9341_CMD_RESET 0x01               // Software reset
#define ILI9341_GET_ID_INFO 0x04             // Get ID information
#define ILI9341_GET_STATUS 0x09              // Get status
#define ILI9341_GET_PWR_MODE 0x0A            // Get power mode
#define ILI9341_GET_MADCTL 0x0B              // Get MADCTL
#define ILI9341_GET_PIX_FMT 0x0C             // Get pixel format
#define ILI9341_GET_IMG_FMT 0x0D             // Get image format
#define ILI9341_GET_SIG_MODE 0x0E            // Get signal mode
#define ILI9341_GET_SELF_DIAG 0x0F           // Get self-diagnostics
#define ILI9341_CMD_SLEEP_ON 0x10            // Enter sleep mode
#define ILI9341_CMD_SLEEP_OFF 0x11           // Exist sleep mode
#define ILI9341_CMD_PARTIAL_ON 0x12          // Enter partial mode
#define ILI9341_CMD_PARTIAL_OFF 0x13         // Exit partial mode
#define ILI9341_CMD_INVERT_ON 0x20           // Enter inverted mode
#define ILI9341_CMD_INVERT_OFF 0x21          // Exit inverted mode
#define ILI9341_SET_GAMMA 0x26               // Set gamma params
#define ILI9341_CMD_DISPLAY_OFF 0x28         // Disable display
#define ILI9341_CMD_DISPLAY_ON 0x29          // Enable display
#define ILI9341_SET_COL_ADDR 0x2A            // Set column address
#define ILI9341_SET_PAGE_ADDR 0x2B           // Set page address
#define ILI9341_SET_MEM 0x2C                 // Set memory
#define ILI9341_SET_COLOR 0x2D               // Set color
#define ILI9341_GET_MEM 0x2E                 // Get memory
#define ILI9341_SET_PARTIAL_AREA 0x30        // Set partial area
#define ILI9341_SET_VSCROLL 0x33             // Set vertical scroll def
#define ILI9341_CMD_TEARING_ON 0x34          // Tearing line enabled
#define ILI9341_CMD_TEARING_OFF 0x35         // Tearing line disabled
#define ILI9341_SET_MEM_ACS_CTL 0x36         // Set mem access ctl
#define ILI9341_SET_VSCROLL_ADDR 0x37        // Set vscroll start addr
#define ILI9341_CMD_IDLE_OFF 0x38            // Exit idle mode
#define ILI9341_CMD_IDLE_ON 0x39             // Enter idle mode
#define ILI9341_SET_PIX_FMT 0x3A             // Set pixel format
#define ILI9341_SET_MEM_CONT 0x3C            // Set memory continue
#define ILI9341_GET_MEM_CONT 0x3E            // Get memory continue
#define ILI9341_SET_TEAR_SCANLINE 0x44       // Set tearing scanline
#define ILI9341_GET_TEAR_SCANLINE 0x45       // Get tearing scanline
#define ILI9341_SET_BRIGHTNESS 0x51          // Set brightness
#define ILI9341_GET_BRIGHTNESS 0x52          // Get brightness
#define ILI9341_SET_DISPLAY_CTL 0x53         // Set display ctl
#define ILI9341_GET_DISPLAY_CTL 0x54         // Get display ctl
#define ILI9341_SET_CABC 0x55                // Set CABC
#define ILI9341_GET_CABC 0x56                // Get CABC
#define ILI9341_SET_CABC_MIN 0x5E            // Set CABC min
#define ILI9341_GET_CABC_MIN 0x5F            // Set CABC max
#define ILI9341_GET_ID1 0xDA                 // Get ID1
#define ILI9341_GET_ID2 0xDB                 // Get ID2
#define ILI9341_GET_ID3 0xDC                 // Get ID3
#define ILI9341_SET_RGB_IF_SIG_CTL 0xB0      // RGB IF signal ctl
#define ILI9341_SET_FRAME_CTL_NORMAL 0xB1    // Set frame ctl (normal)
#define ILI9341_SET_FRAME_CTL_IDLE 0xB2      // Set frame ctl (idle)
#define ILI9341_SET_FRAME_CTL_PARTIAL 0xB3   // Set frame ctl (partial)
#define ILI9341_SET_INVERSION_CTL 0xB4       // Set inversion ctl
#define ILI9341_SET_BLANKING_PORCH_CTL 0xB5  // Set blanking porch ctl
#define ILI9341_SET_FUNCTION_CTL 0xB6        // Set function ctl
#define ILI9341_SET_ENTRY_MODE 0xB7          // Set entry mode
#define ILI9341_SET_LIGHT_CTL_1 0xB8         // Set backlight ctl 1
#define ILI9341_SET_LIGHT_CTL_2 0xB9         // Set backlight ctl 2
#define ILI9341_SET_LIGHT_CTL_3 0xBA         // Set backlight ctl 3
#define ILI9341_SET_LIGHT_CTL_4 0xBB         // Set backlight ctl 4
#define ILI9341_SET_LIGHT_CTL_5 0xBC         // Set backlight ctl 5
#define ILI9341_SET_LIGHT_CTL_7 0xBE         // Set backlight ctl 7
#define ILI9341_SET_LIGHT_CTL_8 0xBF         // Set backlight ctl 8
#define ILI9341_SET_POWER_CTL_1 0xC0         // Set power ctl 1
#define ILI9341_SET_POWER_CTL_2 0xC1         // Set power ctl 2
#define ILI9341_SET_VCOM_CTL_1 0xC5          // Set VCOM ctl 1
#define ILI9341_SET_VCOM_CTL_2 0xC7          // Set VCOM ctl 2
#define ILI9341_POWER_CTL_A 0xCB             // Set power control A
#define ILI9341_POWER_CTL_B 0xCF             // Set power control B
#define ILI9341_DRV_TIMING_CTL_A 0xE8        // Set driver timing control A
#define ILI9341_DRV_TIMING_CTL_B 0xEA        // Set driver timing control B
#define ILI9341_POWER_ON_SEQ_CTL 0xED        // Set Power on sequence control
#define ILI9341_SET_NVMEM 0xD0               // Set NVMEM data
#define ILI9341_GET_NVMEM_KEY 0xD1           // Get NVMEM protect key
#define ILI9341_GET_NVMEM_STATUS 0xD2        // Get NVMEM status
#define ILI9341_GET_ID4 0xD3                 // Get ID4
#define ILI9341_SET_PGAMMA 0xE0              // Set positive gamma
#define ILI9341_SET_NGAMMA 0xE1              // Set negative gamma
#define ILI9341_SET_DGAMMA_CTL_1 0xE2        // Set digital gamma ctl 1
#define ILI9341_SET_DGAMMA_CTL_2 0xE3        // Set digital gamma ctl 2
#define ILI9341_ENABLE_3_GAMMA 0xF2          // Enable 3 gamma
#define ILI9341_SET_IF_CTL 0xF6              // Set interface control
#define ILI9341_SET_PUMP_RATIO_CTL 0xF7      // Set pump ratio control

typedef struct ili9341_painter_device_t {
    struct painter_driver_t qp_driver;  // must be first, so it can be cast from the painter_device_t* type
    bool                    allocated;
    pin_t                   chip_select_pin;
    pin_t                   data_pin;
    pin_t                   reset_pin;
    painter_rotation_t      rotation;
    bool                    uses_backlight;
} ili9341_painter_device_t;

static inline uint16_t hsv_to_ili9341(uint8_t hue, uint8_t sat, uint8_t val) {
    RGB      rgb    = qp_hsv_to_rgb((HSV){hue, sat, val});
    uint16_t rgb565 = (((uint16_t)rgb.r) >> 3) << 11 | (((uint16_t)rgb.g) >> 2) << 5 | (((uint16_t)rgb.b) >> 3);
    return BYTE_SWAP(rgb565);
}

painter_lld_status_t ili9341_qp_init(painter_device_t device, painter_rotation_t rotation);
painter_lld_status_t ili9341_qp_clear(painter_device_t device);
painter_lld_status_t ili9341_qp_power(painter_device_t device, bool power_on);
painter_lld_status_t ili9341_qp_viewport(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
painter_lld_status_t ili9341_qp_pixdata(painter_device_t device, const void *pixel_data, uint32_t byte_count);
painter_lld_status_t ili9341_qp_setpixel(painter_device_t device, uint16_t x, uint16_t y, uint8_t hue, uint8_t sat, uint8_t val);
painter_lld_status_t ili9341_qp_line(painter_device_t device, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t hue, uint8_t sat, uint8_t val);
painter_lld_status_t ili9341_qp_rect(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint8_t hue, uint8_t sat, uint8_t val, bool filled);
painter_lld_status_t ili9341_qp_drawimage(painter_device_t device, uint16_t x, uint16_t y, const painter_image_descriptor_t *image);

static inline void lcd_start(ili9341_painter_device_t *lcd) { spi_start(lcd->chip_select_pin, false, 0, ILI9341_SPI_DIVISOR); }

static inline void lcd_stop(void) { spi_stop(); }

static inline void lcd_cmd(ili9341_painter_device_t *lcd, uint8_t b) {
    writePinLow(lcd->data_pin);
    spi_write(b);
}

static inline void lcd_sendbuf(ili9341_painter_device_t *lcd, const void *data, uint16_t len) {
    writePinHigh(lcd->data_pin);
    spi_transmit(data, len);
}

static inline void lcd_data(ili9341_painter_device_t *lcd, uint8_t b) { lcd_sendbuf(lcd, &b, sizeof(b)); }

static inline void lcd_reg(ili9341_painter_device_t *lcd, uint8_t reg, uint8_t val) {
    lcd_cmd(lcd, reg);
    lcd_data(lcd, val);
}

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

static inline void lcd_send_mono_pixdata_recolour(ili9341_painter_device_t *lcd, uint8_t bits_per_pixel, uint32_t pixel_count, const void *pixel_data, uint32_t byte_count, int16_t hue_fg, int16_t sat_fg, int16_t val_fg, int16_t hue_bg, int16_t sat_bg, int16_t val_bg) {
    uint16_t       buf[ILI9341_PIXDATA_BUFSIZE];
    const uint8_t  pixel_bitmask       = (1 << bits_per_pixel) - 1;
    const uint8_t  pixels_per_byte     = 8 / bits_per_pixel;
    const uint16_t max_transmit_pixels = ((ILI9341_PIXDATA_BUFSIZE / pixels_per_byte) * pixels_per_byte);  // the number of rgb565 pixels that we can complete fit in the buffer
    const uint8_t *pixdata             = (const uint8_t *)pixel_data;
    uint32_t       remaining_pixels    = pixel_count;  // don't try to derive from byte_count, we may not use an entire byte

    // Generate the colour lookup table
    HSV      hsv_lookup_table[16];
    uint16_t rgb565_lookup_table[16];
    uint8_t  items = 1 << bits_per_pixel;  // number of items we need to intepolate
    qp_generate_colour_lookup_table(hsv_lookup_table, items, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
    for (uint8_t i = 0; i < items; ++i) {
        rgb565_lookup_table[i] = hsv_to_ili9341(hsv_lookup_table[i].h, hsv_lookup_table[i].s, hsv_lookup_table[i].v);
    }

    // Transmit each block of pixels
    while (remaining_pixels > 0) {
        uint16_t  transmit_pixels = remaining_pixels < max_transmit_pixels ? remaining_pixels : max_transmit_pixels;
        uint16_t *target16        = (uint16_t *)buf;
        for (uint16_t p = 0; p < transmit_pixels; p += pixels_per_byte) {
            uint8_t pixval      = *pixdata;
            uint8_t loop_pixels = remaining_pixels < pixels_per_byte ? remaining_pixels : pixels_per_byte;
            for (uint8_t q = 0; q < loop_pixels; ++q) {
                *target16++ = rgb565_lookup_table[pixval & pixel_bitmask];
                pixval >>= bits_per_pixel;
            }
            ++pixdata;
        }
        lcd_sendbuf(lcd, buf, transmit_pixels * sizeof(uint16_t));
        remaining_pixels -= transmit_pixels;
    }
}

static inline void lcd_send_mono_pixdata(ili9341_painter_device_t *lcd, uint8_t bits_per_pixel, uint32_t pixel_count, const void *pixel_data, uint32_t byte_count) {
    // Default implementation is greyscale
    lcd_send_mono_pixdata_recolour(lcd, bits_per_pixel, pixel_count, pixel_data, byte_count, 0, 0, 255, 0, 0, 0);
}

painter_lld_status_t ili9341_qp_init(painter_device_t device, painter_rotation_t rotation) {
    static const uint8_t pgamma[15] = {0x0F, 0x29, 0x24, 0x0C, 0x0E, 0x09, 0x4E, 0x78, 0x3C, 0x09, 0x13, 0x05, 0x17, 0x11, 0x00};
    static const uint8_t ngamma[15] = {0x00, 0x16, 0x1B, 0x04, 0x11, 0x07, 0x31, 0x33, 0x42, 0x05, 0x0C, 0x0A, 0x28, 0x2F, 0x0F};

    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;
    lcd->rotation                 = rotation;

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

    lcd_start(lcd);

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

    lcd_cmd(lcd, ILI9341_DRV_TIMING_CTL_A);
    lcd_data(lcd, 0x85);
    lcd_data(lcd, 0x10);
    lcd_data(lcd, 0x7A);

    lcd_cmd(lcd, ILI9341_DRV_TIMING_CTL_B);
    lcd_data(lcd, 0x00);
    lcd_data(lcd, 0x00);

    lcd_cmd(lcd, ILI9341_SET_PUMP_RATIO_CTL);
    lcd_data(lcd, 0x20);

    lcd_cmd(lcd, ILI9341_SET_BRIGHTNESS);
    lcd_data(lcd, 0xFF);

    lcd_cmd(lcd, ILI9341_SET_POWER_CTL_1);
    lcd_data(lcd, 0x26);

    lcd_cmd(lcd, ILI9341_SET_POWER_CTL_2);
    lcd_data(lcd, 0x11);

    lcd_cmd(lcd, ILI9341_SET_VCOM_CTL_1);
    lcd_data(lcd, 0x35);
    lcd_data(lcd, 0x3E);

    lcd_cmd(lcd, ILI9341_SET_VCOM_CTL_2);
    lcd_data(lcd, 0xBE);

    lcd_cmd(lcd, ILI9341_SET_MEM_ACS_CTL);
    lcd_data(lcd, 0x48);

    lcd_cmd(lcd, ILI9341_SET_PIX_FMT);
    lcd_data(lcd, 0x55);

    lcd_cmd(lcd, ILI9341_SET_FRAME_CTL_NORMAL);
    lcd_data(lcd, 0x00);
    lcd_data(lcd, 0x1B);

    lcd_cmd(lcd, ILI9341_SET_FUNCTION_CTL);
    lcd_data(lcd, 0x0A);
    lcd_data(lcd, 0xA2);

    lcd_cmd(lcd, ILI9341_ENABLE_3_GAMMA);
    lcd_data(lcd, 0x00);

    lcd_cmd(lcd, ILI9341_SET_GAMMA);
    lcd_data(lcd, 0x01);

    lcd_cmd(lcd, ILI9341_SET_PGAMMA);
    lcd_sendbuf(lcd, pgamma, sizeof(pgamma));

    lcd_cmd(lcd, ILI9341_SET_NGAMMA);
    lcd_sendbuf(lcd, ngamma, sizeof(ngamma));

    lcd_viewport(lcd, 0, 0, 239, 319);

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

    lcd_cmd(lcd, ILI9341_CMD_SLEEP_OFF);
    wait_ms(20);

    lcd_stop();

    return DRIVER_SUCCESS;
}

painter_lld_status_t ili9341_qp_clear(painter_device_t device) {
    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;

    // Re-init the LCD
    ili9341_qp_init(device, lcd->rotation);

    return DRIVER_SUCCESS;
}

painter_lld_status_t ili9341_qp_power(painter_device_t device, bool power_on) {
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

    return DRIVER_SUCCESS;
}

painter_lld_status_t ili9341_qp_viewport(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) {
    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;
    lcd_start(lcd);

    // Configure where we're going to be rendering to
    lcd_viewport(lcd, left, top, right, bottom);

    lcd_stop();

    return DRIVER_SUCCESS;
}

painter_lld_status_t ili9341_qp_pixdata(painter_device_t device, const void *pixel_data, uint32_t byte_count) {
    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;
    lcd_start(lcd);

    // Stream data to the LCD
    lcd_sendbuf(lcd, pixel_data, byte_count);

    lcd_stop();

    return DRIVER_SUCCESS;
}

painter_lld_status_t ili9341_qp_setpixel(painter_device_t device, uint16_t x, uint16_t y, uint8_t hue, uint8_t sat, uint8_t val) {
    ili9341_painter_device_t *lcd = (ili9341_painter_device_t *)device;
    lcd_start(lcd);

    // Configure where we're going to be rendering to
    lcd_viewport(lcd, x, y, x, y);

    // Convert the color to RGB565 and transmit to the device
    uint16_t buf = hsv_to_ili9341(hue, sat, val);
    lcd_sendbuf(lcd, &buf, sizeof(buf));

    lcd_stop();

    return DRIVER_SUCCESS;
}

painter_lld_status_t ili9341_qp_line(painter_device_t device, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t hue, uint8_t sat, uint8_t val) {
    if (x0 != x1 && y0 != y1) return DRIVER_UNSUPPORTED;  // for now, let the upper layer handle the line drawing algorithm

    // If we're doing horizontal or vertical, just use the optimised rect draw so we don't need to deal with single pixels or buffers.
    return ili9341_qp_rect(device, x0, y0, x1, y1, hue, sat, val, true);
}

painter_lld_status_t ili9341_qp_rect(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint8_t hue, uint8_t sat, uint8_t val, bool filled) {
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
        ili9341_qp_rect(device, left, top, right, top, hue, sat, val, true);
        ili9341_qp_rect(device, left, bottom, right, bottom, hue, sat, val, true);
        ili9341_qp_rect(device, left, top + 1, left, bottom - 1, hue, sat, val, true);
        ili9341_qp_rect(device, right, top + 1, right, bottom - 1, hue, sat, val, true);
    }

    return DRIVER_SUCCESS;
}

painter_lld_status_t ili9341_qp_drawimage(painter_device_t device, uint16_t x, uint16_t y, const painter_image_descriptor_t *image) {
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
            } else if (image->image_format == IMAGE_FORMAT_MONO4BPP) {
                // Supplied pixel data is in 4bpp monochrome -- decode it to the equivalent pixel data
                uint32_t pixels_this_loop = last_chunk ? pixel_count : (comp_image_desc->chunk_size * 8 / 4);
                lcd_send_mono_pixdata(lcd, 4, pixels_this_loop, buf, decompressed_size);
                pixel_count -= pixels_this_loop;
            } else if (image->image_format == IMAGE_FORMAT_MONO2BPP) {
                // Supplied pixel data is in 2bpp monochrome -- decode it to the equivalent pixel data
                uint32_t pixels_this_loop = last_chunk ? pixel_count : (comp_image_desc->chunk_size * 8 / 2);
                lcd_send_mono_pixdata(lcd, 2, pixels_this_loop, buf, decompressed_size);
                pixel_count -= pixels_this_loop;
            } else if (image->image_format == IMAGE_FORMAT_MONO1BPP) {
                // Supplied pixel data is in 1bpp monochrome -- decode it to the equivalent pixel data
                uint32_t pixels_this_loop = last_chunk ? pixel_count : (comp_image_desc->chunk_size * 8 / 1);
                lcd_send_mono_pixdata(lcd, 1, pixels_this_loop, buf, decompressed_size);
                pixel_count -= pixels_this_loop;
            }
        }
    } else if (image->compression == IMAGE_UNCOMPRESSED) {
        const painter_raw_image_descriptor_t *raw_image_desc = (const painter_raw_image_descriptor_t *)image;
        // Stream data to the LCD
        if (image->image_format == IMAGE_FORMAT_RAW || image->image_format == IMAGE_FORMAT_RGB565) {
            // The pixel data is in the correct format already -- send it directly to the device
            lcd_sendbuf(lcd, raw_image_desc->image_data, raw_image_desc->byte_count);
        } else if (image->image_format == IMAGE_FORMAT_MONO4BPP) {
            // Supplied pixel data is in 4bpp monochrome -- decode it to the equivalent pixel data
            lcd_send_mono_pixdata(lcd, 4, pixel_count, raw_image_desc->image_data, raw_image_desc->byte_count);
        } else if (image->image_format == IMAGE_FORMAT_MONO2BPP) {
            // Supplied pixel data is in 2bpp monochrome -- decode it to the equivalent pixel data
            lcd_send_mono_pixdata(lcd, 2, pixel_count, raw_image_desc->image_data, raw_image_desc->byte_count);
        } else if (image->image_format == IMAGE_FORMAT_MONO1BPP) {
            // Supplied pixel data is in 1bpp monochrome -- decode it to the equivalent pixel data
            lcd_send_mono_pixdata(lcd, 1, pixel_count, raw_image_desc->image_data, raw_image_desc->byte_count);
        }
    }

    lcd_stop();

    return DRIVER_SUCCESS;
}

ili9341_painter_device_t drivers[ILI9341_NUM_DEVICES] = {0};

painter_device_t qp_make_ili9341_device(pin_t chip_select_pin, pin_t data_pin, pin_t reset_pin, bool uses_backlight) {
    for (uint32_t i = 0; i < ILI9341_NUM_DEVICES; ++i) {
        ili9341_painter_device_t *driver = &drivers[i];
        memset(driver, 0, sizeof(ili9341_painter_device_t));
        if (!driver->allocated) {
            driver->allocated           = true;
            driver->qp_driver.init      = ili9341_qp_init;
            driver->qp_driver.clear     = ili9341_qp_clear;
            driver->qp_driver.power     = ili9341_qp_power;
            driver->qp_driver.pixdata   = ili9341_qp_pixdata;
            driver->qp_driver.viewport  = ili9341_qp_viewport;
            driver->qp_driver.setpixel  = ili9341_qp_setpixel;
            driver->qp_driver.line      = ili9341_qp_line;
            driver->qp_driver.rect      = ili9341_qp_rect;
            driver->qp_driver.drawimage = ili9341_qp_drawimage;
            driver->chip_select_pin     = chip_select_pin;
            driver->data_pin            = data_pin;
            driver->reset_pin           = reset_pin;
            driver->uses_backlight      = uses_backlight;
            return (painter_device_t)driver;
        }
    }
    return NULL;
}
