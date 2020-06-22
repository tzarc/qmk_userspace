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

    // Turn on the backlight
    backlight_enable();
    backlight_level(BACKLIGHT_LEVELS);

    // Initialise the LCD
    lcd = make_ili9341_driver(LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN);
    drawable_init(lcd, DRAWABLE_ROTATION_0);
    drawable_power(lcd, true);

    for (int r = 0; r < 320; ++r) {
        HSV      hsv = {r * 255 / 320, 255, 255};
        RGB      rgb = hsv_to_rgb(hsv);
        uint16_t pix_data[240];
        for (int c = 0; c < 240; ++c) {
            pix_data[c] = (rgb.r >> 3) << 11 | (rgb.g >> 2) << 5 | (rgb.b >> 3);
        }

        drawable_viewport(lcd, 0, r, 319, r);
        drawable_pixdata(lcd, pix_data, 240);
    }
}
