#include QMK_KEYBOARD_H

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        BL_STEP, RGB_TOG, RESET,
        RGB_MOD, RGB_HUI, RGB_MODE_RAINBOW,
        KC_A,    KC_B,    KC_C
    )
};
// clang-format on
