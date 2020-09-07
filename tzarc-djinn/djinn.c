/* Copyright 2018-2020 Nick Brassel (@tzarc)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#include "djinn.h"
#include "qp_ili9341.h"
#include "rgblight_list.h"
#include "color.h"

#include "gfx-djinn.c"
#define IMAGE gfx_djinn

void matrix_io_delay(void) { __asm__ volatile("nop\nnop\nnop\n"); }

painter_device_t lcd;

/*
bool is_keyboard_master(void) {
    setPinInput(SPLIT_PLUG_DETECT_PIN);
    return readPin(SPLIT_PLUG_DETECT_PIN);
}
*/

void keyboard_post_init_kb(void) {
    debug_enable = true;
    debug_matrix = true;

    // Turn on the LCD
    setPinOutput(LCD_POWER_ENABLE_PIN);
    writePinHigh(LCD_POWER_ENABLE_PIN);

    // Turn on the RGB
    setPinOutput(RGB_POWER_ENABLE_PIN);
    writePinHigh(RGB_POWER_ENABLE_PIN);

    // Let the LCD get some power...
    wait_ms(50);

    // Initialise the LCD
    lcd = qp_ili9341_make_device(LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN, true);
    qp_init(lcd, QP_ROTATION_0);

    // Turn on the LCD
    qp_power(lcd, true);

#define NUM_ROWS (320 - IMAGE->height)
#define NUM_COLS (240)
    for (int r = 0; r < 320; ++r) {
        uint8_t pix_data[2 * NUM_COLS] = {0};
        if (r < NUM_ROWS) {
            for (int c = 0; c < NUM_COLS; ++c) {
                HSV      hsv        = {r * 255 / NUM_ROWS, 255, c * 255 / NUM_COLS};
                RGB      rgb        = hsv_to_rgb(hsv);
                uint16_t pixel      = (rgb.r >> 3) << 11 | (rgb.g >> 2) << 5 | (rgb.b >> 3);
                pix_data[c * 2 + 0] = pixel >> 8;
                pix_data[c * 2 + 1] = pixel & 0xFF;
            }
        }

        qp_viewport(lcd, 0, r, NUM_COLS - 1, r);
        qp_pixdata(lcd, pix_data, sizeof(pix_data));
    }

    // Test drawing
    qp_line(lcd, 60, 130, 240 - 60, 130, HSV_BLUE);
    qp_rect(lcd, 20, 20, 120, 100, HSV_RED, true);
    qp_rect(lcd, 20, 20, 120, 100, HSV_WHITE, false);
    qp_drawimage(lcd, (240 - IMAGE->width) / 2, 320 - IMAGE->height, IMAGE);

    // Turn on the backlight
    backlight_enable();
    backlight_level(BACKLIGHT_LEVELS);
}
