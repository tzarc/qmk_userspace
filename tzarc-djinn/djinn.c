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

#include <hal.h>
#include "djinn.h"
#include "rgblight_list.h"
#include "color.h"
#include "serial_usart_userxfer.h"

#ifdef QUANTUM_PAINTER_ENABLE
#    include "qp_ili9341.h"
#    include "gfx-djinn.c"
#    define IMAGE gfx_djinn
painter_device_t lcd;
#endif

void matrix_io_delay(void) { __asm__ volatile("nop\nnop\nnop\n"); }

extern bool is_keyboard_left(void);

bool is_keyboard_master(void) {
    static bool determined = false;
    static bool is_master;
    if (!determined) {
        determined = true;
        setPinInputLow(SPLIT_PLUG_DETECT_PIN);
        wait_ms(10);
        is_master = readPin(SPLIT_PLUG_DETECT_PIN) ? true : false;
        if (!is_master) {
            usbStop(&USBD1);
        }
    }

    return is_master;
}

void keyboard_post_init_kb(void) {
    debug_enable = true;
    debug_matrix = true;

    // Force a read initially so that the SPI driver is initialised and the backlight driver doesn't choke
    eeconfig_read_user();

#ifdef RGBLIGHT_ENABLE
    // Turn off increased current limits
    setPinOutput(RGB_CURR_1500mA_OK_PIN);
    writePinLow(RGB_CURR_1500mA_OK_PIN);
    setPinOutput(RGB_CURR_3000mA_OK_PIN);
    writePinLow(RGB_CURR_3000mA_OK_PIN);
    // Turn on the RGB
    setPinOutput(RGB_POWER_ENABLE_PIN);
    writePinHigh(RGB_POWER_ENABLE_PIN);
#endif

#ifdef QUANTUM_PAINTER_ENABLE
    // Turn on the LCD
    setPinOutput(LCD_POWER_ENABLE_PIN);
    writePinHigh(LCD_POWER_ENABLE_PIN);

    // Let the LCD get some power...
    wait_ms(50);

    // Initialise the LCD
    lcd = qp_ili9341_make_device(LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN, 4, true);
    qp_init(lcd, is_keyboard_left() ? QP_ROTATION_0 : QP_ROTATION_180);

    // Turn on the LCD
    qp_power(lcd, true);

#    define NUM_ROWS (320 - IMAGE->height)
#    define NUM_COLS (240)
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
#endif

#ifdef BACKLIGHT_ENABLE
    // Turn on the backlight
    backlight_enable();
    backlight_level(BACKLIGHT_LEVELS);
#endif
}

void encoder_update_user(uint8_t index, bool clockwise);
void encoder_update_kb(uint8_t index, bool clockwise) { encoder_update_user(index, clockwise); }

uint8_t prng(void) {
    static uint8_t s = 0xAA, a = 0;
    s ^= s << 3;
    s ^= s >> 5;
    s ^= a++ >> 2;
    return s;
}

void keyboard_post_init_user(void) {
    // Customise these values to desired behaviour
    debug_enable = true;
    // debug_matrix = true;
    // debug_keyboard=true;
    // debug_mouse=true;
}

void matrix_scan_user(void) {
    static uint32_t last_eeprom_access = 0;
    uint32_t        now                = timer_read32();
    if (now - last_eeprom_access > 60000) {
        dprint("reading eeprom\n");
        last_eeprom_access = now;

        union {
            uint8_t  bytes[4];
            uint32_t raw;
        } tmp;
        tmp.bytes[0] = prng();
        tmp.bytes[1] = prng();
        tmp.bytes[2] = prng();
        tmp.bytes[3] = prng();

        eeconfig_update_user(tmp.raw);
        uint32_t value = eeconfig_read_user();
        if (value != tmp.raw) {
            dprint("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            dprint("!! EEPROM readback mismatch!\n");
            dprint("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            while (1)
                ;
        }
    }
}

static struct data_xfer { uint8_t value; } data_xfer;

bool serial_userxfer_receive(const void* data, size_t len) {
    struct data_xfer* xfer = (struct data_xfer*)data;
    xfer->value++;
    serial_userxfer_respond(data, sizeof(struct data_xfer));
    return true;
}

void housekeeping_task_kb(void) {
    static uint32_t last_sync = 0;
    uint32_t        now       = timer_read32();
    if (now - last_sync > 5000) {
        last_sync = now;

        if (is_keyboard_master()) {
            dprint("Sync'ing data with slave\n");
            dprintf("Before: %d\n", (int)data_xfer.value);
            serial_userxfer_transaction(&data_xfer, sizeof(data_xfer), &data_xfer, sizeof(data_xfer));
            dprintf("After: %d\n", (int)data_xfer.value);
        }
    }
}

static bool current_1500mA = false;
static bool current_3000mA = false;

RGB rgblight_hsv_to_rgb(HSV hsv) {
    float scale = 0.25f;
    if (current_3000mA) {
        scale = 0.60f;
    } else if (current_1500mA) {
        scale = 0.40f;
    }

    hsv.v = (uint8_t)(hsv.v * scale);
    return hsv_to_rgb(hsv);
}
