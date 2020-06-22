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
#include <spi_master.h>
#include "drawable_ili9341.h"

#ifndef ILI9341_NUM_DEVICES
#    define ILI9341_NUM_DEVICES 1
#endif

#ifndef ILI9341_SPI_DIVISOR
#    define ILI9341_SPI_DIVISOR 8
#endif

#ifndef ILI9341_PIXDATA_BUFSIZE
#    define ILI9341_PIXDATA_BUFSIZE 16
#endif

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

typedef struct ili9341_drawable_driver_t {
    drawable_driver_t   drawable_driver;  // must be first, so it can be cast from the drawable_driver_t* type
    bool                allocated;
    pin_t               chip_select_pin;
    pin_t               data_pin;
    pin_t               reset_pin;
    drawable_rotation_t rotation;
} ili9341_drawable_driver_t;

static inline void lcd_start(ili9341_drawable_driver_t *lcd) { spi_start(lcd->chip_select_pin, false, 0, ILI9341_SPI_DIVISOR); }

static inline void lcd_stop(void) { spi_stop(); }

static inline void lcd_cmd(ili9341_drawable_driver_t *lcd, uint8_t b) {
    writePinLow(lcd->data_pin);
    spi_write(b);
}

static inline void lcd_sendbuf(ili9341_drawable_driver_t *lcd, const void *data, uint16_t len) {
    writePinHigh(lcd->data_pin);
    spi_transmit(data, len);
}

static inline void lcd_data(ili9341_drawable_driver_t *lcd, uint8_t b) { lcd_sendbuf(lcd, &b, sizeof(b)); }

static inline void lcd_reg(ili9341_drawable_driver_t *lcd, uint8_t reg, uint8_t val) {
    lcd_cmd(lcd, reg);
    lcd_data(lcd, val);
}

static inline void lcd_viewport(ili9341_drawable_driver_t *lcd, uint16_t xbegin, uint16_t ybegin, uint16_t xend, uint16_t yend) {
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

void ili9341_drawable_init_func(drawable_driver_t *driver, drawable_rotation_t rotation) {
    static const uint8_t pgamma[15] = {0x0F, 0x29, 0x24, 0x0C, 0x0E, 0x09, 0x4E, 0x78, 0x3C, 0x09, 0x13, 0x05, 0x17, 0x11, 0x00};
    static const uint8_t ngamma[15] = {0x00, 0x16, 0x1B, 0x04, 0x11, 0x07, 0x31, 0x33, 0x42, 0x05, 0x0C, 0x0A, 0x28, 0x2F, 0x0F};

    ili9341_drawable_driver_t *lcd = (ili9341_drawable_driver_t *)driver;
    lcd->rotation                  = rotation;

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

    lcd_cmd(lcd, ILI9341_CMD_SLEEP_OFF);
    wait_ms(20);

    lcd_stop();
}

void ili9341_drawable_clear_func(drawable_driver_t *driver) {
    ili9341_drawable_driver_t *lcd = (ili9341_drawable_driver_t *)driver;

    // Re-init the LCD
    ili9341_drawable_init_func(driver, lcd->rotation);
}

void ili9341_drawable_power_func(drawable_driver_t *driver, bool power_on) {
    ili9341_drawable_driver_t *lcd = (ili9341_drawable_driver_t *)driver;
    lcd_start(lcd);
    lcd_cmd(lcd, power_on ? ILI9341_CMD_DISPLAY_ON : ILI9341_CMD_DISPLAY_OFF);
    lcd_stop();
}

void ili9341_drawable_viewport_func(drawable_driver_t *driver, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) {
    ili9341_drawable_driver_t *lcd = (ili9341_drawable_driver_t *)driver;
    lcd_start(lcd);
    lcd_viewport(lcd, left, top, right, bottom);
    lcd_stop();
}

void ili9341_drawable_pixdata_func(drawable_driver_t *driver, const uint16_t *pixel_data, uint32_t num_pixels) {
    ili9341_drawable_driver_t *lcd = (ili9341_drawable_driver_t *)driver;
    lcd_start(lcd);
    uint8_t temp[ILI9341_PIXDATA_BUFSIZE * 2];
    while (num_pixels > 0) {
        uint32_t to_draw = (num_pixels < ILI9341_PIXDATA_BUFSIZE) ? num_pixels : ILI9341_PIXDATA_BUFSIZE;
        for (uint32_t p = 0; p < to_draw; ++p) {
            temp[p * 2 + 0] = pixel_data[p] >> 8;
            temp[p * 2 + 1] = pixel_data[p] & 0xFF;
        }

        lcd_sendbuf(lcd, temp, to_draw * 2);
        pixel_data += to_draw;
        num_pixels -= to_draw;
    }

    lcd_stop();
}

void ili9341_drawable_setpixel_func(drawable_driver_t *driver, uint16_t x, uint16_t y, uint16_t color) {
    ili9341_drawable_driver_t *lcd = (ili9341_drawable_driver_t *)driver;
    lcd_start(lcd);
    lcd_viewport(lcd, x, y, x, y);
    uint8_t buf[2] = {color >> 8, color & 0xFF};
    lcd_sendbuf(lcd, &buf, sizeof(buf));
    lcd_stop();
}

void ili9341_drawable_line_func(drawable_driver_t *driver, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
    ili9341_drawable_driver_t *lcd = (ili9341_drawable_driver_t *)driver;
    lcd_start(lcd);
    // do stuff
    lcd_stop();
}

void ili9341_drawable_rect_func(drawable_driver_t *driver, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint16_t color, bool filled) {
    ili9341_drawable_driver_t *lcd = (ili9341_drawable_driver_t *)driver;
    lcd_start(lcd);
    // do stuff
    lcd_stop();
}

ili9341_drawable_driver_t drivers[ILI9341_NUM_DEVICES] = {0};

drawable_driver_t *make_ili9341_driver(pin_t chip_select_pin, pin_t data_pin, pin_t reset_pin) {
    for (int i = 0; i < ILI9341_NUM_DEVICES; ++i) {
        ili9341_drawable_driver_t *driver = &drivers[i];
        if (!driver->allocated) {
            driver->allocated                = true;
            driver->drawable_driver.init     = ili9341_drawable_init_func;
            driver->drawable_driver.clear    = ili9341_drawable_clear_func;
            driver->drawable_driver.power    = ili9341_drawable_power_func;
            driver->drawable_driver.pixdata  = ili9341_drawable_pixdata_func;
            driver->drawable_driver.viewport = ili9341_drawable_viewport_func;
            driver->drawable_driver.setpixel = ili9341_drawable_setpixel_func;
            driver->drawable_driver.line     = ili9341_drawable_line_func;
            driver->drawable_driver.rect     = ili9341_drawable_rect_func;
            driver->chip_select_pin          = chip_select_pin;
            driver->data_pin                 = data_pin;
            driver->reset_pin                = reset_pin;
            return (drawable_driver_t *)driver;
        }
    }
    return NULL;
}
