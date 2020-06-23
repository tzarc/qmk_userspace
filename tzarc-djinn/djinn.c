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
#include "drawable_ili9341.h"
#include "color.h"
#include "qmk-logo-200.h"

void matrix_io_delay(void) { __asm__ volatile("nop\nnop\nnop\n"); }

drawable_driver_t *lcd;

void keyboard_post_init_kb(void) {
    debug_enable = true;
    debug_matrix = true;

    // Turn on the LCD
    setPinOutput(LCD_POWER_ENABLE_PIN);
    writePinLow(LCD_POWER_ENABLE_PIN);

    // Turn on the RGB
    setPinOutput(RGB_POWER_ENABLE_PIN);
    writePinLow(RGB_POWER_ENABLE_PIN);

    // Initialise the LCD
    lcd = make_ili9341_driver(LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN);
    drawable_init(lcd, DRAWABLE_ROTATION_270);

    for (int r = 0; r < 240; ++r) {
        uint8_t pix_data[2 * 320] = {0};
        if (r < 160) {
            for (int c = 0; c < 320; ++c) {
                HSV      hsv        = {r*255/160, c*255/320, 255};
                RGB      rgb        = hsv_to_rgb(hsv);
                uint16_t pixel      = (rgb.r >> 3) << 11 | (rgb.g >> 2) << 5 | (rgb.b >> 3);
                pix_data[c * 2 + 0] = pixel >> 8;
                pix_data[c * 2 + 1] = pixel & 0xFF;
            }
        }

        drawable_viewport(lcd, 0, r, 319, r);
        drawable_pixdata(lcd, (const uint8_t *)pix_data, 320 * 2);
    }

    drawable_viewport(lcd, 0, 160, 199, 239);
    drawable_pixdata(lcd, qmk_logo_200_rgb565, qmk_logo_200_rgb565_len);
    drawable_power(lcd, true);

    // Turn on the backlight
    backlight_enable();
    backlight_level(BACKLIGHT_LEVELS);
}
