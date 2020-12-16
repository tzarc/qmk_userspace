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

#include <string.h>
#include <hal.h>
#include "djinn.h"
#include "rgblight_list.h"
#include "color.h"
#include "serial_usart_dataxfer.h"

#include "qp_ili9341.h"

kb_runtime_config kb_conf;
painter_device_t  lcd;

#ifdef SPLIT_KEYBOARD
bool serial_dataxfer_receive_kb(const void* data, size_t len) {
    const kb_runtime_config* xfer = (const kb_runtime_config*)data;
    kb_conf.raw                   = xfer->raw;
    serial_dataxfer_respond_kb(&kb_conf, sizeof(kb_conf));
    return true;
}
#endif  // SPLIT_KEYBOARD

void housekeeping_task_kb(void) {
#ifdef SPLIT_KEYBOARD
    // If we're the master side, then propagate our runtime config to the slave
    if (is_keyboard_master()) {
        static uint32_t          last_sync = 0;
        static kb_runtime_config last_data;

        // Turn off the LCD if there's been no matrix activity
        kb_conf.values.lcd_power = (last_matrix_activity_elapsed() < LCD_ACTIVITY_TIMEOUT) ? 1 : 0;

        // Send the data from the master to the slave
        uint32_t now = timer_read32();
        if (now - last_sync > 2500 || last_data.raw != kb_conf.raw) {  // At worst, resync every 250ms
            last_sync     = now;
            last_data.raw = kb_conf.raw;

            kb_runtime_config slave_runtime_cfg;
            slave_runtime_cfg.raw = 0;
            serial_dataxfer_transaction_kb(&kb_conf, sizeof(kb_conf), &slave_runtime_cfg, sizeof(slave_runtime_cfg));
        }
    }
#endif  // SPLIT_KEYBOARD

    static uint8_t current_setting = current_500mA;
    if (current_setting != kb_conf.values.current_setting) {
        current_setting = kb_conf.values.current_setting;
        switch (current_setting) {
            default:
            case current_500mA:
                writePinLow(RGB_CURR_1500mA_OK_PIN);
                writePinLow(RGB_CURR_3000mA_OK_PIN);
                break;
            case current_1500mA:
                writePinHigh(RGB_CURR_1500mA_OK_PIN);
                writePinLow(RGB_CURR_3000mA_OK_PIN);
                break;
            case current_3000mA:
                writePinHigh(RGB_CURR_1500mA_OK_PIN);
                writePinHigh(RGB_CURR_3000mA_OK_PIN);
                break;
        }
    }

    static bool lcd_on = false;
    if (lcd_on != (bool)kb_conf.values.lcd_power) {
        lcd_on = (bool)kb_conf.values.lcd_power;
        qp_power(lcd, lcd_on);
    }

    if (is_backlight_enabled() != lcd_on) {
        if (lcd_on)
            backlight_enable();
        else
            backlight_disable();
    }
}

//----------------------------------------------------------
// LCD power control

void djinn_lcd_on(void) {
    dprint("djinn_lcd_on\n");
    kb_conf.values.lcd_power = 1;
}

void djinn_lcd_off(void) {
    dprint("djinn_lcd_off\n");
    kb_conf.values.lcd_power = 0;
}

void djinn_lcd_toggle(void) {
    if (kb_conf.values.lcd_power)
        djinn_lcd_off();
    else
        djinn_lcd_on();
}

//----------------------------------------------------------
// Initialisation

void keyboard_post_init_kb(void) {
    debug_enable = true;

    // Reset the initial shared data value between master and slave
    kb_conf.raw = 0;

    // Turn off increased current limits
    setPinOutput(RGB_CURR_1500mA_OK_PIN);
    writePinLow(RGB_CURR_1500mA_OK_PIN);
    setPinOutput(RGB_CURR_3000mA_OK_PIN);
    writePinLow(RGB_CURR_3000mA_OK_PIN);

    // Turn on the RGB
    setPinOutput(RGB_POWER_ENABLE_PIN);
    writePinHigh(RGB_POWER_ENABLE_PIN);

    // Turn on the LCD
    setPinOutput(LCD_POWER_ENABLE_PIN);
    writePinHigh(LCD_POWER_ENABLE_PIN);

    // Let the LCD get some power...
    wait_ms(50);

    // Initialise the LCD
    lcd = qp_ili9341_make_device(LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN, 4, true);
    qp_init(lcd, QP_ROTATION_0);

    // Turn on the LCD and draw the logo
    djinn_lcd_on();
    qp_power(lcd, true);
    qp_rect(lcd, 0, 0, 239, 319, 0, 0, 0, true);

    // Turn on the LCD backlight
    backlight_enable();
    backlight_level(BACKLIGHT_LEVELS);
}

//----------------------------------------------------------
// QMK overrides

// Disable plug detection for now, as it's not working for some people.
#if 0
#    ifdef SPLIT_KEYBOARD
bool is_keyboard_master(void) {
    static bool determined = false;
    static bool is_master;
    if (!determined) {
        determined = true;
        setPinInputLow(SPLIT_PLUG_DETECT_PIN);
        wait_ms(50);
        is_master = readPin(SPLIT_PLUG_DETECT_PIN) ? true : false;
        if (!is_master) {
            usbStop(&USBD1);
        }
    }

    return is_master;
}
#    endif  // SPLIT_KEYBOARD
#endif

void matrix_io_delay(void) {
    for (int i = 0; i < 250; ++i) {
        // Give the matrix scanning a small amount of time to settle.
        __asm__ volatile("nop\nnop\nnop\n");
    }
}

void encoder_update_kb(uint8_t index, bool clockwise) {
    // Offload to the keymap instead.
    extern void encoder_update_user(uint8_t index, bool clockwise);
    encoder_update_user(index, clockwise);
}

void suspend_power_down_kb(void) {
    suspend_power_down_user();

    // djinn_lcd_off();
    // qp_power(lcd, false);
    // writePinLow(LCD_POWER_ENABLE_PIN);
}

void suspend_wakeup_init_kb(void) {
    // writePinHigh(LCD_POWER_ENABLE_PIN);
    // wait_ms(50);
    // djinn_lcd_on();
    // qp_init(lcd, QP_ROTATION_0);
    // qp_power(lcd, true);
    // qp_clear(lcd);
    // qp_rect(lcd, 0, 0, 239, 319, 0, 0, 0, true);

    suspend_wakeup_init_user();
}

RGB rgblight_hsv_to_rgb(HSV hsv) {
    float scale;
    switch (kb_conf.values.current_setting) {
        default:
        case current_500mA:
            scale = 0.25f;
            break;
        case current_1500mA:
            scale = 0.40f;
            break;
        case current_3000mA:
            scale = 0.60f;
            break;
    }

    hsv.v = (uint8_t)(hsv.v * scale);
    return hsv_to_rgb(hsv);
}
