#include QMK_KEYBOARD_H
#include "qp.h"

#define MEDIA_KEY_DELAY 10

enum { KC_TOGGLE_LCD_POWER = SAFE_RANGE };

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_ESC,  KC_1, KC_2, KC_3, KC_4, KC_5, KC_GRV,                                                         KC_DEL,   KC_6,  KC_7,    KC_8,    KC_9,   KC_0,    KC_BSPC,
        KC_TAB,  KC_Q, KC_W, KC_E, KC_R, KC_T, KC_LBRC,                                                        KC_RBRC,  KC_Y,  KC_U,    KC_I,    KC_O,   KC_P,    KC_BSLS,
        KC_LCTL, KC_A, KC_S, KC_D, KC_F, KC_G, KC_HOME,                                                        KC_PGUP,  KC_H,  KC_J,    KC_K,    KC_L,   KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_END,                                                         KC_PGDN,  KC_N,  KC_M,    KC_COMM, KC_DOT, KC_SLSH, KC_ENT,
                        KC_NO, KC_LGUI, KC_NO, KC_SPACE,                                                       KC_SPACE, KC_NO, KC_LALT, KC_NO,
                                                         BL_STEP, KC_MUTE, RESET,    EEP_RST, KC_MUTE, RESET,
                                    KC_UP,                               RGB_RMOD,   RGB_MOD,                                KC_UP,
                           KC_LEFT, KC_MUTE, KC_RIGHT,                                                              KC_LEFT, KC_MUTE, KC_RIGHT,
                                    KC_DOWN,                                                                                 KC_DOWN
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

void encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) { /* First encoder */
        uint16_t held_keycode_timer = timer_read();
        uint16_t mapped_code = 0;
        if (clockwise) {
            mapped_code = KC_VOLD;
        } else {
            mapped_code = KC_VOLU;
        }

        register_code(mapped_code);
        while (timer_elapsed(held_keycode_timer) < MEDIA_KEY_DELAY){ /* no-op */ }
        unregister_code(mapped_code);
    } else if (index == 1) { /* Second encoder */
        if (clockwise) {
            rgblight_increase_hue_noeeprom();
        } else {
            rgblight_decrease_hue_noeeprom();
        }
    }
}