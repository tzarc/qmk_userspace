/* Copyright 2021 Nick Brassel (@tzarc)
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
#include "serial_usart_statesync.h"

#include "qp_ili9341.h"
#include "qp_rgb565_surface.h"

painter_device_t lcd;
painter_device_t surf;

kb_runtime_config kb_state;
uint32_t          last_slave_sync_time = 0;

void board_init(void) { usbpd_init(); }

void* get_split_sync_state_kb(size_t* state_size) {
    *state_size = sizeof(kb_runtime_config);
    return &kb_state;
}

bool split_sync_update_task_kb(void) {
    if (is_keyboard_master()) {
        // Turn off the LCD if there's been no matrix activity
        kb_state.values.lcd_power = (last_input_activity_elapsed() < LCD_ACTIVITY_TIMEOUT) ? 1 : 0;
    } else {
        last_slave_sync_time = timer_read32();
    }

    // Force an update if the state changed
    static kb_runtime_config last_state;
    if (memcmp(&last_state, &kb_state, sizeof(kb_runtime_config)) != 0) {
        memcpy(&last_state, &kb_state, sizeof(kb_runtime_config));
        return true;
    }

    return false;
}

void split_sync_action_task_kb(void) {
    // Work out if we've changed our current limit, update the limiter circuit switches
    static uint8_t current_setting = USBPD_500MA;
    if (current_setting != kb_state.values.current_setting) {
        current_setting = kb_state.values.current_setting;
        switch (current_setting) {
            default:
            case USBPD_500MA:
                writePinLow(RGB_CURR_1500mA_OK_PIN);
                writePinLow(RGB_CURR_3000mA_OK_PIN);
                break;
            case USBPD_1500MA:
                writePinHigh(RGB_CURR_1500mA_OK_PIN);
                writePinLow(RGB_CURR_3000mA_OK_PIN);
                break;
            case USBPD_3000MA:
                writePinHigh(RGB_CURR_1500mA_OK_PIN);
                writePinHigh(RGB_CURR_3000mA_OK_PIN);
                break;
        }

        // Toggle rgblight on and off, if it's already on, to force a brightness update on all LEDs
        if (rgblight_is_enabled()) {
            rgblight_disable_noeeprom();
            rgblight_enable_noeeprom();
        }
    }

    // Turn on/off the LCD
    static bool lcd_on = false;
    if (lcd_on != (bool)kb_state.values.lcd_power) {
        lcd_on = (bool)kb_state.values.lcd_power;
        qp_power(lcd, lcd_on);
    }

    // Match the backlight to the LCD state
    if (is_backlight_enabled() != lcd_on) {
        if (lcd_on)
            backlight_enable();
        else
            backlight_disable();
    }
}

const char* usbpd_str(usbpd_allowance_t allowance) {
    switch (allowance) {
        case USBPD_500MA:
            return "500mA";
        case USBPD_1500MA:
            return "1500mA";
        case USBPD_3000MA:
            return "3000mA";
        default:
            return "500mA";
    }
}

void usbpd_task_kb(void) {
    if (is_keyboard_master()) {
        static uint32_t last_read = 0;
        if (timer_elapsed32(last_read) > 250) {
            last_read = timer_read32();
            switch (usbpd_get_allowance()) {
                case USBPD_500MA:
                    if (kb_state.values.current_setting != USBPD_500MA) {
                        dprintf("Transitioning UCPD1 %s -> %s\n", usbpd_str(kb_state.values.current_setting), usbpd_str(USBPD_500MA));
                        kb_state.values.current_setting = USBPD_500MA;
                    }
                    break;
                case USBPD_1500MA:
                    if (kb_state.values.current_setting != USBPD_1500MA) {
                        dprintf("Transitioning UCPD1 %s -> %s\n", usbpd_str(kb_state.values.current_setting), usbpd_str(USBPD_1500MA));
                        kb_state.values.current_setting = USBPD_1500MA;
                    }
                    break;
                case USBPD_3000MA:
                    if (kb_state.values.current_setting != USBPD_3000MA) {
                        dprintf("Transitioning UCPD1 %s -> %s\n", usbpd_str(kb_state.values.current_setting), usbpd_str(USBPD_3000MA));
                        kb_state.values.current_setting = USBPD_3000MA;
                    }
                    break;
            }
        }
    }
}
void housekeeping_task_kb(void) {
    // Modify current limits
    usbpd_task_kb();

    // Ensure state is sync'ed between master and slave, if required
    split_sync_kb(false);
}

//----------------------------------------------------------
// Initialisation

void keyboard_post_init_kb(void) {
    debug_enable = true;
    // debug_matrix = true;

    // Reset the initial shared data value between master and slave
    kb_state.raw = 0;

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

    // Initialise the framebuffer
    surf = qp_rgb565_surface_make_device(8, 320);
    qp_init(surf, QP_ROTATION_0);
    for (int i = 0; i < 320; ++i) {
        qp_line(surf, 0, i, 7, i, i % 256, 255, 255);
    }

    // Initialise the LCD
    lcd = qp_ili9341_make_device(LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN, 4, true);
    qp_init(lcd, QP_ROTATION_0);

    // Turn on the LCD and clear the display
    kb_state.values.lcd_power = 1;
    qp_power(lcd, true);
    qp_rect(lcd, 0, 0, 239, 319, HSV_BLACK, true);

    qp_viewport(lcd, 240 - 8 - 8, 0, 240 - 8 - 1, 319);
    qp_pixdata(lcd, qp_rgb565_surface_get_buffer_ptr(surf), qp_rgb565_surface_get_pixel_count(surf));

    // Turn on the LCD backlight
    backlight_enable();
    backlight_level(BACKLIGHT_LEVELS);
}

//----------------------------------------------------------
// QMK overrides

#ifdef USE_PLUG_DETECT_PIN
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
#endif  // USE_PLUG_DETECT_PIN

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
    switch (kb_state.values.current_setting) {
        default:
        case USBPD_500MA:
            scale = 0.3f;
            break;
        case USBPD_1500MA:
            scale = 0.65f;
            break;
        case USBPD_3000MA:
            scale = 1.0f;
            break;
    }

    hsv.v = (uint8_t)(hsv.v * scale);
    return hsv_to_rgb(hsv);
}

#ifdef RGB_MATRIX_ENABLE
// clang-format off
#define RLO 42
#define LLI(x) (x)
#define LLP(x,y) {(x),(y)}
#define RLI(x) (RLO-(x))
#define RLP(x,y) {(224-(x)),((y))}
led_config_t g_led_config = {
    {
        // Key Matrix to LED Index
        { LLI(35), LLI(36), LLI(37), LLI(38), LLI(39), LLI(40), LLI(41) },
        { LLI(34), LLI(33), LLI(32), LLI(31), LLI(30), LLI(29), LLI(28) },
        { LLI(21), LLI(22), LLI(23), LLI(24), LLI(25), LLI(26), LLI(27) },
        { LLI(20), LLI(19), LLI(18), LLI(17), LLI(16), LLI(15), LLI(14) },
        {  NO_LED,  NO_LED,  NO_LED, LLI(10), LLI(11), LLI(12), LLI(13) },
        {  NO_LED,  NO_LED,  NO_LED,  NO_LED, NO_LED,  NO_LED,  NO_LED, },
        { RLI(35), RLI(36), RLI(37), RLI(38), RLI(39), RLI(40), RLI(41) },
        { RLI(34), RLI(33), RLI(32), RLI(31), RLI(30), RLI(29), RLI(28) },
        { RLI(21), RLI(22), RLI(23), RLI(24), RLI(25), RLI(26), RLI(27) },
        { RLI(20), RLI(19), RLI(18), RLI(17), RLI(16), RLI(15), RLI(14) },
        {  NO_LED,  NO_LED,  NO_LED, RLI(10), RLI(11), RLI(12), RLI(13) },
        {  NO_LED,  NO_LED,  NO_LED,  NO_LED, NO_LED,  NO_LED,  NO_LED, },
    },
    {
        // LED Index to Physical Position
        // Underglow left
        LLP(110, 30), LLP(110, 80), LLP(110, 130), LLP(95, 180), LLP(75, 195), LLP(60, 210), LLP(45, 190), LLP(35, 175), LLP(20, 160), LLP(5, 160),
        // Matrix left
        LLP(78, 150), LLP(104, 150), LLP(130, 150), LLP(156, 150),
        LLP( 0, 150), LLP( 26, 150), LLP( 52, 150), LLP( 78, 150), LLP( 104, 150), LLP(130, 150), LLP(156, 150),
        LLP( 0, 150), LLP( 26, 150), LLP( 52, 150), LLP( 78, 150), LLP( 104, 150), LLP(130, 150), LLP(156, 150),
        LLP( 0, 150), LLP( 26, 150), LLP( 52, 150), LLP( 78, 150), LLP( 104, 150), LLP(130, 150), LLP(156, 150),
        LLP( 0, 150), LLP( 26, 150), LLP( 52, 150), LLP( 78, 150), LLP( 104, 150), LLP(130, 150), LLP(156, 150),
        // Underglow right
        RLP(110, 30), RLP(110, 80), RLP(110, 130), RLP(95, 180), RLP(75, 195), RLP(60, 210), RLP(45, 190), RLP(35, 175), RLP(20, 160), RLP(5, 160),
        // Matrix right
        RLP(78, 150), RLP(104, 150), RLP(130, 150), RLP(156, 150),
        RLP( 0, 150), RLP( 26, 150), RLP( 52, 150), RLP( 78, 150), RLP(104, 150), RLP(130, 150), RLP(156, 150),
        RLP( 0, 150), RLP( 26, 150), RLP( 52, 150), RLP( 78, 150), RLP(104, 150), RLP(130, 150), RLP(156, 150),
        RLP( 0, 150), RLP( 26, 150), RLP( 52, 150), RLP( 78, 150), RLP(104, 150), RLP(130, 150), RLP(156, 150),
        RLP( 0, 150), RLP( 26, 150), RLP( 52, 150), RLP( 78, 150), RLP(104, 150), RLP(130, 150), RLP(156, 150),
    },
    {
        // LED Index to Flag
        // Underglow left
        LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW,
        // Matrix left
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        // Underglow right
        LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW,
        // Matrix right
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
    }
};
// clang-format on
#endif  // RGB_MATRIX_ENABLE
