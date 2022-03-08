// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <string.h>
#include <quantum.h>
#include <hal_pal.h>

#include "djinn.h"
#include "serial.h"
#include "split_util.h"

#include "qp_ili9341.h"

painter_device_t lcd;

void board_init(void) {
    usbpd_init();
}

__attribute__((weak)) void draw_ui_user(void) {}

void housekeeping_task_kb(void) {
    // Update kb_state so we can send to slave
    kb_state_update();

    // Data sync from master to slave
    kb_state_sync();

    // Work out if we've changed our current limit, update the limiter circuit switches
    static uint8_t current_setting = USBPD_500MA;
    if (current_setting != kb_state.current_setting) {
        current_setting = kb_state.current_setting;
        switch (current_setting) {
            default:
            case USBPD_500MA:
                writePinLow(RGB_CURR_1500mA_OK_PIN);
                writePinLow(RGB_CURR_3000mA_OK_PIN);
                break;
#ifdef DJINN_SUPPORTS_3A_FUSE
            case USBPD_1500MA:
                writePinHigh(RGB_CURR_1500mA_OK_PIN);
                writePinLow(RGB_CURR_3000mA_OK_PIN);
                break;
            case USBPD_3000MA:
                writePinHigh(RGB_CURR_1500mA_OK_PIN);
                writePinHigh(RGB_CURR_3000mA_OK_PIN);
                break;
#else
            case USBPD_1500MA:
            case USBPD_3000MA:
                writePinHigh(RGB_CURR_1500mA_OK_PIN);
                writePinLow(RGB_CURR_3000mA_OK_PIN);
                break;
#endif
        }

        // Toggle rgblight on and off, if it's already on, to force a brightness update on all LEDs
        if (is_keyboard_master() && rgblight_is_enabled()) {
            rgblight_disable_noeeprom();
            rgblight_enable_noeeprom();
        }
    }

    // Turn on/off the LCD
    static bool lcd_on = false;
    if (lcd_on != (bool)kb_state.lcd_power) {
        lcd_on = (bool)kb_state.lcd_power;
        qp_power(lcd, lcd_on);
    }

    // Enable/disable RGB
    if (lcd_on) {
        // Turn on RGB_PWR
        writePinHigh(RGB_POWER_ENABLE_PIN);
        // Modify the RGB state if different to the LCD state
        if (rgblight_is_enabled() != lcd_on) {
            // Wait for a small amount of time to allow the RGB capacitors to charge, before enabling RGB output
            wait_ms(10);
            // Enable RGB
            rgblight_enable_noeeprom();
        }
    } else {
        // Turn off RGB_PWR
        writePinLow(RGB_POWER_ENABLE_PIN);
        // Disable the PWM output for the RGB
        if (rgblight_is_enabled() != lcd_on) {
            rgblight_disable_noeeprom();
        }
    }

    // Match the backlight to the LCD state
    if (is_keyboard_master() && is_backlight_enabled() != lcd_on) {
        if (lcd_on)
            backlight_enable();
        else
            backlight_disable();
    }

    // Draw the UI
    if (kb_state.lcd_power) {
        draw_ui_user();
    }

    // Go into low-scan interrupt-based mode if we haven't had any matrix activity in the last 5 seconds
    if (last_input_activity_elapsed() > 5000) {
        matrix_wait_for_interrupt();
    }
}

//----------------------------------------------------------
// Initialisation

void keyboard_post_init_kb(void) {
    // Register keyboard state sync split transaction
    transaction_register_rpc(RPC_ID_SYNC_STATE_KB, kb_state_sync_slave);

    // Reset the initial shared data value between master and slave
    memset(&kb_state, 0, sizeof(kb_state));

    // Turn off increased current limits
    setPinOutput(RGB_CURR_1500mA_OK_PIN);
    writePinLow(RGB_CURR_1500mA_OK_PIN);
    setPinOutput(RGB_CURR_3000mA_OK_PIN);
    writePinLow(RGB_CURR_3000mA_OK_PIN);

    // Turn on the RGB
    setPinOutput(RGB_POWER_ENABLE_PIN);
    writePinHigh(RGB_POWER_ENABLE_PIN);

#ifdef EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN
    setPinOutput(EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN);
    writePinHigh(EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN);
#endif // EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN

    // Turn on the LCD
    setPinOutput(LCD_POWER_ENABLE_PIN);
    writePinHigh(LCD_POWER_ENABLE_PIN);

    // Let the LCD get some power...
    wait_ms(150);

    // Initialise the LCD
    lcd = qp_ili9341_make_spi_device(320, 240, LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN, 4, 3);
    qp_init(lcd, QP_ROTATION_0);

    // Turn on the LCD and clear the display
    kb_state.lcd_power = 1;
    qp_power(lcd, true);
    qp_rect(lcd, 0, 0, 239, 319, HSV_BLACK, true);

    // Turn on the LCD backlight
    backlight_enable();
    backlight_level(BACKLIGHT_LEVELS);

    // Allow for user post-init
    keyboard_post_init_user();
}

//----------------------------------------------------------
// QMK overrides

#if defined(RGB_MATRIX_ENABLE)
RGB rgb_matrix_hsv_to_rgb(HSV hsv) {
    float scale;
    switch (kb_state.current_setting) {
        default:
        case USBPD_500MA:
            scale = 0.35f;
            break;
#    ifdef DJINN_SUPPORTS_3A_FUSE
        case USBPD_1500MA:
            scale = 0.75f;
            break;
        case USBPD_3000MA:
            scale = 1.0f;
            break;
#    else
        case USBPD_1500MA:
        case USBPD_3000MA:
            scale = 0.75f;
            break;
#    endif
    }

    hsv.v = (uint8_t)(hsv.v * scale);
    return hsv_to_rgb(hsv);
}
#endif