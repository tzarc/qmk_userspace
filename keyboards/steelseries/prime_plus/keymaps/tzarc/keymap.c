// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <math.h>
#include QMK_KEYBOARD_H
#include "tzarc.h"
#include "tzarc_layout.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT(
        KC_MS_BTN1, KC_MS_BTN3, KC_MS_BTN2, // L, M, R
            LT(LAYER_RAISE, KC_MS_BTN5),    //    F
            LT(LAYER_LOWER, KC_MS_BTN4),    //    B
                    TIME_RESET              //   CPI
    ),
    [LAYER_LOWER] = LAYOUT(
        TZ_TBMS1, KC_TRNS, KC_TRNS,
                  KC_TRNS,
                  KC_TRNS,
                  KC_TRNS
    ),
    [LAYER_RAISE] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS,
                 KC_TRNS,
                 KC_TRNS,
                 KC_TRNS
    ),
    [LAYER_ADJUST] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS,
                 KC_TRNS,
                 KC_TRNS,
                 KC_TRNS
    ),
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [LAYER_BASE]   = {ENCODER_CCW_CW(KC_MS_WH_UP, KC_MS_WH_DOWN)},
    [LAYER_LOWER]  = {ENCODER_CCW_CW(RGB_HUD, RGB_HUI)},
    [LAYER_RAISE]  = {ENCODER_CCW_CW(RGB_MOD, RGB_RMOD)},
    [LAYER_ADJUST] = {ENCODER_CCW_CW(KC_NO, KC_NO)},
};
#endif
// clang-format on

void keyboard_post_init_keymap(void) {
    debug_enable = true;
}

void pointing_device_init_user(void) {
    pointing_device_set_cpi(6400);
}

static bool    lower_pressed    = false;
static bool    raise_pressed    = false;
static bool    gesture_pressed  = false;
static bool    gesture_actioned = true;
static int32_t start_position_x = 0;
static int32_t start_position_y = 0;

static bool gesture_mode(layer_state_t state) {
    return get_highest_layer(state) == LAYER_RAISE;
}

bool pre_process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(LAYER_LOWER, KC_MS_BTN4):
            lower_pressed = record->event.pressed;
            break;
        case LT(LAYER_RAISE, KC_MS_BTN5):
            raise_pressed = record->event.pressed;
            break;
    }

    return true;
}

static const int turbo_mouse_low_rng         = 50;
static const int turbo_mouse_high_rng        = 110;
static bool      turbo_mouse_keydown         = false;
static bool      turbo_mouse_released        = false;
static bool      turbo_mouse_auto_registered = false;
static uint32_t  turbo_mouse_last_keydown    = 0;
static uint32_t  turbo_mouse_next_trigger    = 0;

bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TZ_TBMS1: {
            uint32_t now = timer_read32();
            // Keep track of if this key is held down
            turbo_mouse_keydown = record->event.pressed;

            if (record->event.pressed) {
                // Keydown event, clear the released flag
                turbo_mouse_released = false;

                // Keep track of the last keydown event, as well as next trigger time
                turbo_mouse_last_keydown = now;
                turbo_mouse_next_trigger = now + prng(turbo_mouse_low_rng, turbo_mouse_high_rng);

                // Inform the OS that we've got a keydown event
                register_code(KC_MS_BTN1);
            } else {
                // If the release happened within the initial hold period, then stop the timer and tap the key as per normal
                if ((now < turbo_mouse_next_trigger && !turbo_mouse_released) || turbo_mouse_auto_registered) {
                    unregister_code(KC_MS_BTN1);
                    turbo_mouse_auto_registered = false;
                }
            }
            return false;
        }
        default:
            break;
    }
    return true;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // One layer tap has kicked in, start accumulating mouse movement
    if (gesture_mode(layer_state | default_layer_state)) {
        start_position_x += mouse_report.x;
        start_position_y += mouse_report.y;
    }

    // If we're holding raise, disable any mouse movement
    if (raise_pressed) {
        mouse_report.x = 0;
        mouse_report.y = 0;
        mouse_report.v = 0;
        mouse_report.h = 0;
    }
    return mouse_report;
}

layer_state_t layer_state_set_keymap(layer_state_t state) {
    if (gesture_mode(state)) {
        start_position_x = 0;
        start_position_y = 0;
        gesture_pressed  = true;
        gesture_actioned = false;
    } else {
        gesture_pressed = false;
    }
    return state;
}

void housekeeping_task_keymap(void) {
    uint32_t now = timer_read32();

    // If this key is held down, and we've passed the trigger point...
    if (turbo_mouse_keydown && turbo_mouse_next_trigger <= now) {
        // Check if we've not yet released due to the initial hold period
        if (!turbo_mouse_released) {
            // ...if we haven't, then we release the key.
            unregister_code(KC_MS_BTN1);
            turbo_mouse_released        = true;
            turbo_mouse_auto_registered = false;
        } else {
            // Toggle the keypress with random timing below
            if (!turbo_mouse_auto_registered) {
                register_code(KC_MS_BTN1);
                turbo_mouse_auto_registered = true;
            } else {
                unregister_code(KC_MS_BTN1);
                turbo_mouse_auto_registered = false;
            }
        }

        // Set the next trigger.
        turbo_mouse_next_trigger = now + prng(turbo_mouse_low_rng, turbo_mouse_high_rng);
    }

    // Handle cardinal direction gestures
    if (!gesture_pressed && !gesture_actioned) {
        gesture_actioned = true;

        if (sqrtf(start_position_x * start_position_x + start_position_y * start_position_y) < 500) {
            return;
        }

        float r  = atan2f(start_position_y, start_position_x);
        float d  = 180 * r / M_PI;
        int   id = (int)d;

        const int   directional_split = 8;
        const char *directions[]      = {"E", "SE", "S", "SW", "W", "NW", "N", "NE"};
        int         direction_idx     = ((id + 360 + 360 / directional_split / 2) % 360 / (360 / directional_split)) % directional_split;
        uprintf("Mouse movement: %d, %d => %d degrees (%s)\n", (int)start_position_x, (int)start_position_y, id, directions[direction_idx]);

        switch (direction_idx) {
            case 0: // E
                tap_code(KC_F19);
                break;
            case 1: // SE
                tap_code(KC_F22);
                break;
            case 2: // S
                tap_code(KC_F21);
                break;
            case 3: // SW
                tap_code(KC_F20);
                break;
            case 4: // W
                tap_code(KC_F17);
                break;
            case 5: // NW
                tap_code(KC_F14);
                break;
            case 6: // N
                tap_code(KC_F15);
                break;
            case 7: // NE
                tap_code(KC_F16);
                break;
        }
    }
}
