// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "theme_djinn_default.h"
#include "tzarc.h"
#include "qp.h"
// #include "qp_rgb565_surface.h"
// #include "qp_lvgl.h"
// #include "ui.h"

#if defined(FILESYSTEM_ENABLE) && defined(EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN)
#    include "filesystem.h"
#endif // defined(FILESYSTEM_ENABLE) && defined(EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN)

//----------------------------------------------------------
// Key map

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT_all_wrapper(
        ____________TZARC_7x4_BASE_R1_L__________,                                            ____________TZARC_7x4_BASE_R1_R__________,
        ____________TZARC_7x4_BASE_R2_L__________,                                            ____________TZARC_7x4_BASE_R2_R__________,
        ____________TZARC_7x4_BASE_R3_L__________,                                            ____________TZARC_7x4_BASE_R3_R__________,
        ____________TZARC_7x4_BASE_R4_L__________,                                            ____________TZARC_7x4_BASE_R4_R__________,
                     KC_LGUI, KC_LOWER,  KC_SPC,  CHANGEMODE,                       CHANGEMODE,  KC_SPC,  KC_RAISE,  KC_LALT,
                                                           TZ_ENC1P,          TZ_ENC2P,
                                KC_UP,                                                                 KC_UP,
                       KC_LEFT, KC_NUM,  KC_RIGHT,                                            KC_LEFT, KC_NUM,  KC_RIGHT,
                                KC_DOWN,                                                               KC_DOWN
    ),
    [LAYER_LOWER] = LAYOUT_all_wrapper(
        ____________TZARC_7x4_LOWER_R1_L_________,                                            ____________TZARC_7x4_LOWER_R1_R_________,
        ____________TZARC_7x4_LOWER_R2_L_________,                                            ____________TZARC_7x4_LOWER_R2_R_________,
        ____________TZARC_7x4_LOWER_R3_L_________,                                            ____________TZARC_7x4_LOWER_R3_R_________,
        ____________TZARC_7x4_LOWER_R4_L_________,                                            ____________TZARC_7x4_LOWER_R4_R_________,
                     _______, _______, _______, _______,                                _______, _______, _______, KC_RALT,
                                                           BL_DOWN,           BL_UP,
                                _______,                                                             _______,
                       _______, _______, _______,                                           _______, _______, _______,
                                _______,                                                             _______
    ),
    [LAYER_RAISE] = LAYOUT_all_wrapper(
        ____________TZARC_7x4_RAISE_R1_L_________,                                            ____________TZARC_7x4_RAISE_R1_R_________,
        ____________TZARC_7x4_RAISE_R2_L_________,                                            ____________TZARC_7x4_RAISE_R2_R_________,
        ____________TZARC_7x4_RAISE_R3_L_________,                                            ____________TZARC_7x4_RAISE_R3_R_________,
        ____________TZARC_7x4_RAISE_R4_L_________,                                            ____________TZARC_7x4_RAISE_R4_R_________,
                     _______, _______, _______, _______,                                _______, _______, _______, _______,
                                                           _______,           _______,
                                _______,                                                             _______,
                       _______, _______, _______,                                           _______, _______, _______,
                                _______,                                                             _______
    ),
    [LAYER_ADJUST] = LAYOUT_all_wrapper(
        ____________TZARC_7x4_ADJUST_R1_L________,                                            ____________TZARC_7x4_ADJUST_R1_R________,
        ____________TZARC_7x4_ADJUST_R2_L________,                                            ____________TZARC_7x4_ADJUST_R2_R________,
        ____________TZARC_7x4_ADJUST_R3_L________,                                            ____________TZARC_7x4_ADJUST_R3_R________,
        ____________TZARC_7x4_ADJUST_R4_L________,                                            ____________TZARC_7x4_ADJUST_R4_R________,
                     _______, _______, _______, _______,                                _______, _______, _______, _______,
                                                           _______,           CM_HELO,
                                _______,                                                             _______,
                       _______, _______, _______,                                           _______, _______, _______,
                                _______,                                                             _______
    )
};
// clang-format on

//----------------------------------------------------------
// Encoder map

// clang-format off
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [LAYER_BASE]   = { ENCODER_CCW_CW(TZ_ENC1L, TZ_ENC1R), ENCODER_CCW_CW(TZ_ENC2L, TZ_ENC2R) },
    [LAYER_LOWER]  = { ENCODER_CCW_CW(XRGB_HUD, XRGB_HUI), ENCODER_CCW_CW(XRGB_SAD, XRGB_SAI) },
    [LAYER_RAISE]  = { ENCODER_CCW_CW(XRGB_VAD, XRGB_VAI), ENCODER_CCW_CW(XRGB_SPD, XRGB_SPI) },
    [LAYER_ADJUST] = { ENCODER_CCW_CW(XRGB_PRV, XRGB_NXT), ENCODER_CCW_CW(KC_LEFT, KC_RIGHT)  },
};
// clang-format on

//----------------------------------------------------------
// Layer naming

const char *current_layer_name(void) {
    switch (get_highest_layer(layer_state)) {
        case LAYER_BASE:
            return "qwerty";
        case LAYER_LOWER:
            return "lower";
        case LAYER_RAISE:
            return "raise";
        case LAYER_ADJUST:
            return "adjust";
    }
    return "unknown";
}

//----------------------------------------------------------
// Overrides

#if defined(FILESYSTEM_ENABLE) && defined(EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN)
static bool is_mounted = false;
#endif // defined(FILESYSTEM_ENABLE) && defined(EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN)

void keyboard_post_init_keymap(void) {
    // Initialise the theme
    theme_init();

    void keyboard_post_init_display(void);
    keyboard_post_init_display();
    rgb_matrix_disable_noeeprom();

#if defined(FILESYSTEM_ENABLE) && defined(EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN)
    is_mounted = fs_init();
#endif // defined(FILESYSTEM_ENABLE) && defined(EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN)
}

bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TZ_ENC1P:
            if (record->event.pressed) {
                rgb_matrix_step_reverse();
            }
            return false;
        case TZ_ENC1L:
            if (record->event.pressed) {
                tap_code16(KC_MS_WH_UP);
            }
            return false;
        case TZ_ENC1R:
            if (record->event.pressed) {
                tap_code16(KC_MS_WH_DOWN);
            }
            return false;
        case TZ_ENC2P:
            if (record->event.pressed) {
                rgb_matrix_step();
            }
            return false;
        case TZ_ENC2L:
            if (record->event.pressed) {
                tap_code16_delay(KC_VOLD, 2);
            }
            return false;
        case TZ_ENC2R:
            if (record->event.pressed) {
                tap_code16_delay(KC_VOLU, 2);
            }
            return false;
    }
    return true;
}

void housekeeping_task_keymap(void) {
    // Update kb_state so we can send to slave
    theme_state_update();

    // Data sync from master to slave
    theme_state_sync();

#if defined(FILESYSTEM_ENABLE) && defined(EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN)
    if (is_mounted) {
        static uint32_t minutes_running = 0;
        if (timer_elapsed32(minutes_running) > 60000) {
            minutes_running = timer_read32();
            fs_fd_t fd      = fs_open("minutes", FS_READ | FS_WRITE);
            if (fd != INVALID_FILESYSTEM_FD) {
                uint32_t minutes = 0;
                if (fs_read(fd, &minutes, sizeof(minutes)) != sizeof(minutes)) {
                    minutes = 0;
                }
                ++minutes;
                fs_seek(fd, 0, FS_SEEK_SET);
                fs_write(fd, &minutes, sizeof(minutes));
                fs_close(fd);
                dprintf("Minutes running: %d\n", (int)minutes);
            }
        }

        // Dump info
        static bool testing = false;
        if (!testing) {
            if (timer_read32() > 15000) {
                testing = true;

                extern void fs_dump_info(void);
                fs_dump_info();

                // Test recursive directory creation and deletion
                fs_mkdir("a");
                fs_mkdir("a/b");
                fs_mkdir("a/b/c");
                fs_fd_t fd = fs_open("a/z", FS_WRITE);
                fs_write(fd, &testing, sizeof(testing));
                fs_close(fd);
                fd = fs_open("a/b/y", FS_WRITE);
                fs_write(fd, &testing, sizeof(testing));
                fs_close(fd);
                fd = fs_open("a/b/c/x", FS_WRITE);
                fs_write(fd, &testing, sizeof(testing));
                fs_close(fd);
                fs_rmdir("a", true);
            }
        }
    }
#endif // defined(FILESYSTEM_ENABLE) && defined(EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN)
}

#ifdef DEBUG_EEPROM_OUTPUT

#    ifdef WEAR_LEVELING_ENABLE
#        include "wear_leveling.h"
#    endif // WEAR_LEVELING_ENABLE

void matrix_scan_keymap(void) {
    static uint32_t last_eeprom_access = 0;
    uint32_t        now                = timer_read32();
    if (now - last_eeprom_access > 5000) {
        dprint("reading eeprom\n");
        last_eeprom_access = now;

        union {
            uint8_t  bytes[4];
            uint32_t raw;
        } tmp;
        tmp.bytes[0] = prng8();
        tmp.bytes[1] = prng8();
        tmp.bytes[2] = prng8();
        tmp.bytes[3] = prng8();

        eeconfig_update_kb(tmp.raw);
        uint32_t value = eeconfig_read_kb();
        if (value != tmp.raw) {
            dprint("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            dprint("!! EEPROM readback mismatch!\n");
            dprint("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        }
    }

#    ifdef WEAR_LEVELING_ENABLE
    static uint32_t last_wear_leveling_init = 0;
    if (now - last_wear_leveling_init > 30000) {
        dprint("init'ing wear-leveling to simulate power reset\n");
        last_wear_leveling_init = now;
        wear_leveling_init();
    }
#    endif // WEAR_LEVELING_ENABLE
}

#endif // DEBUG_EEPROM_OUTPUT
