#include QMK_KEYBOARD_H

enum { _BASE = 0, _LAYER1, _LAYER2, _LAYER3, _LAYER4 };

#define LAYER1 MO(_LAYER1)
#define LAYER2 MO(_LAYER2)
#define LAYER3 MO(_LAYER3)
#define LAYER4 MO(_LAYER4)

enum {
    TBL_FLIP = SAFE_RANGE,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_BASE] = LAYOUT_ortho_4x4(
    LAYER1,   KC_HOME,  KC_UP,    KC_END ,
    LAYER2,   KC_LEFT,  KC_DOWN,  KC_RGHT,
    LAYER3,   KC_VOLU,  KC_MPLY,  KC_MPRV,
    LAYER4,   KC_VOLD,  KC_MUTE,  KC_MNXT
  ),
  [_LAYER1] = LAYOUT_ortho_4x4(
    _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______
  ),
  [_LAYER2] = LAYOUT_ortho_4x4(
    _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______
  ),
  [_LAYER3] = LAYOUT_ortho_4x4(
    _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______
  ),
  [_LAYER4] = LAYOUT_ortho_4x4(
    _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,
    _______,  _______,  _______,  _______,
    _______,  _______,  _______,  TBL_FLIP
  )
};
// clang-format on

void keyboard_post_init_keymap(void) { set_unicode_input_mode(UC_WINC); }

bool process_record_keymap(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
        case TBL_FLIP:
            if (record->event.pressed) {
                send_unicode_string("(ノಠ痊ಠ)ノ彡┻━┻");
            }
            return false;
    }
    return true;
}
