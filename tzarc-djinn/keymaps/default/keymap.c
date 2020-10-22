#include QMK_KEYBOARD_H
#ifdef QUANTUM_PAINTER_ENABLE
#    include "qp.h"
#endif

#define MEDIA_KEY_DELAY 2

enum { _QWERTY, _LOWER, _RAISE, _ADJUST };
#define KC_LWR MO(_LOWER)
#define KC_RSE MO(_RAISE)

enum { KC_TOGGLE_LCD_POWER = SAFE_RANGE };

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] = LAYOUT(
        KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,    KC_GRV,                                             KC_DEL,  KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,    KC_LBRC,                                            KC_RBRC, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
        KC_LCTL,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,    KC_HOME,                                            KC_PGUP, KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,    KC_END,                                             KC_PGDN, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
                                   KC_NO,   KC_LGUI, KC_LWR,  KC_SPACE,                                           KC_SPC,  KC_RSE,  KC_LALT, KC_NO,
                                                                      RGB_RMOD,                           RGB_MOD,
                                                     KC_UP,                                                                KC_UP,
                                            KC_LEFT, KC_MUTE, KC_RIGHT,                                           KC_LEFT, KC_MUTE, KC_RIGHT,
                                                     KC_DOWN,                                                              KC_DOWN
    ),
    [_LOWER] = LAYOUT(
        KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______,                                           _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
        _______, _______, KC_UP,   _______, _______, _______, _______,                                           _______, _______, _______, _______, _______, _______, _______,
        _______, KC_LEFT, KC_DOWN, KC_RIGHT,_______, _______, _______,                                           _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,                                           _______, _______, _______, _______, _______, _______, _______,
                                   _______, _______, _______, _______,                                           _______, _______, _______, _______,
                                                                     BL_DEC,                               BL_INC,
                                                     _______,                                                             _______,
                                            _______, _______, _______,                                           _______, _______, _______,
                                                     _______,                                                             _______
    ),
    [_RAISE] = LAYOUT(
        KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______,                                           _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
        _______,_______, KC_UP,    _______, _______, _______, _______,                                           _______, _______, _______, _______, _______, _______, _______,
        _______,KC_LEFT, KC_DOWN, KC_RIGHT, _______, KC_UNDS, _______,                                           _______, KC_EQL,  _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, KC_MINS, _______,                                           _______, KC_PLUS, _______, _______, _______, _______, _______,
                                   _______, _______, _______, _______,                                           _______, _______, _______, _______,
                                                                     _______,                             _______,
                                                     _______,                                                             _______,
                                            _______, _______, _______,                                           _______, _______, _______,
                                                     _______,                                                             _______
    ),
    [_ADJUST] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______,                                           _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,                                           _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,                                           _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______,                                           _______, _______, _______, _______, _______, _______, _______,
                                   _______, _______, _______, _______,                                           _______, _______, _______, _______,
                                                                     _______,                             _______,
                                                     _______,                                                             _______,
                                            _______, _______, _______,                                           _______, _______, _______,
                                                     _______,                                                             _______
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

layer_state_t layer_state_set_user(layer_state_t state) {
    // Default handler for lower/raise/adjust
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

void encoder_update_user(uint8_t index, bool clockwise) {
    uint8_t temp_mod   = get_mods();
    uint8_t temp_osm   = get_oneshot_mods();
    bool    is_shifted = (temp_mod | temp_osm) & MOD_MASK_SHIFT;

    if (!is_shifted) {
        if (index == 0) { /* First encoder */
            if (clockwise) {
                rgblight_increase_hue_noeeprom();
            } else {
                rgblight_decrease_hue_noeeprom();
            }
        } else if (index == 1) { /* Second encoder */
            uint16_t held_keycode_timer = timer_read();
            uint16_t mapped_code        = 0;
            if (clockwise) {
                mapped_code = KC_VOLD;
            } else {
                mapped_code = KC_VOLU;
            }
            register_code(mapped_code);
            while (timer_elapsed(held_keycode_timer) < MEDIA_KEY_DELAY)
                ; /* no-op */
            unregister_code(mapped_code);
        }
    } else {
        if (index == 0) { /* First encoder */
            if (clockwise) {
                rgblight_increase_val_noeeprom();
            } else {
                rgblight_decrease_val_noeeprom();
            }
        } else if (index == 1) { /* Second encoder */
            if (clockwise) {
                rgblight_decrease_sat_noeeprom();
            } else {
                rgblight_increase_sat_noeeprom();
            }
        }
    }
}