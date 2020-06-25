#include QMK_KEYBOARD_H
#include "qp.h"

enum { KC_TOGGLE_LCD_POWER = SAFE_RANGE };

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        BL_STEP, RGB_TOG, KC_TOGGLE_LCD_POWER,
        RGB_MOD, RGB_HUI, RGB_MODE_RAINBOW,
        KC_A,    KC_B,    KC_C
    )
};
// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static bool lcd_power = true;
    switch (keycode) {
        case KC_TOGGLE_LCD_POWER:
            if (record->event.pressed) {
                lcd_power = !lcd_power;
                extern painter_device_t *lcd;
                qp_power(lcd, lcd_power);
            }
            break;
    }
    return true;
}