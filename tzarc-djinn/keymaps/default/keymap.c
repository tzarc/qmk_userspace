#include QMK_KEYBOARD_H
#include "qp.h"

enum { KC_TOGGLE_LCD_POWER = SAFE_RANGE };

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_GRV,
        KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_LBRC,
        KC_LCTL, KC_A, KC_S, KC_D, KC_F, KC_G, KC_HOME,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_END,
                        KC_NO, KC_LGUI, KC_NO, KC_SPACE,
                                                         BL_STEP, RGB_MOD, RESET,
                                    KC_UP,
                            KC_LEFT, KC_NO, KC_RIGHT,
                                    KC_DOWN
    )
};
// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static bool lcd_power = true;
    switch (keycode) {
        case KC_TOGGLE_LCD_POWER:
            if (record->event.pressed) {
                lcd_power = !lcd_power;
                extern painter_device_t lcd;
                qp_power(lcd, lcd_power);
            }
            break;
    }
    return true;
}
