// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#include QMK_KEYBOARD_H
#include "theme_djinn_default.h"
#include "tzarc.h"
#include "qp.h"
//#include "qp_rgb565_surface.h"
//#include "qp_lvgl.h"
//#include "ui.h"

//----------------------------------------------------------
// Key map

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT_all_wrapper(
        ____________TZARC_7x4_BASE_R1_L__________,                                            ____________TZARC_7x4_BASE_R1_R__________,
        ____________TZARC_7x4_BASE_R2_L__________,                                            ____________TZARC_7x4_BASE_R2_R__________,
        ____________TZARC_7x4_BASE_R3_L__________,                                            ____________TZARC_7x4_BASE_R3_R__________,
        ____________TZARC_7x4_BASE_R4_L__________,                                            ____________TZARC_7x4_BASE_R4_R__________,
                     KC_LGUI, KC_LOWER,  KC_SPC,  KC_CONFIG,                         KC_CONFIG,  KC_SPC,  KC_RAISE,  KC_LALT,
                                                           TZ_ENC1P,          TZ_ENC2P,
                                KC_UP,                                                                 KC_UP,
                       KC_LEFT, KC_NLCK, KC_RIGHT,                                            KC_LEFT, KC_NLCK, KC_RIGHT,
                                KC_DOWN,                                                               KC_DOWN
    ),
    [LAYER_LOWER] = LAYOUT_all_wrapper(
        ____________TZARC_7x4_LOWER_R1_L_________,                                            ____________TZARC_7x4_LOWER_R1_R_________,
        ____________TZARC_7x4_LOWER_R2_L_________,                                            ____________TZARC_7x4_LOWER_R2_R_________,
        ____________TZARC_7x4_LOWER_R3_L_________,                                            ____________TZARC_7x4_LOWER_R3_R_________,
        ____________TZARC_7x4_LOWER_R4_L_________,                                            ____________TZARC_7x4_LOWER_R4_R_________,
                     _______, _______, _______, _______,                                _______, _______, _______, KC_RALT,
                                                           BL_DEC,            BL_INC,
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
                                                           _______,           _______,
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
    [LAYER_LOWER]  = { ENCODER_CCW_CW(RGB_HUD, RGB_HUI),   ENCODER_CCW_CW(RGB_SAD, RGB_SAI)   },
    [LAYER_RAISE]  = { ENCODER_CCW_CW(RGB_VAD, RGB_VAI),   ENCODER_CCW_CW(RGB_SPD, RGB_SPI)   },
    [LAYER_ADJUST] = { ENCODER_CCW_CW(RGB_RMOD, RGB_MOD),  ENCODER_CCW_CW(KC_LEFT, KC_RIGHT)  },
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

void keyboard_post_init_keymap(void) {
    // Initialise the theme
    theme_init();

    void keyboard_post_init_display(void);
    keyboard_post_init_display();
    rgb_matrix_disable_noeeprom();

    // qp_lvgl_attach(lcd);
    // ui_init();
}

bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TZ_ENC1P:
            if (record->event.pressed) {
                rgb_matrix_step();
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
                rgb_matrix_step_reverse();
            }
            return false;
        case TZ_ENC2L:
            if (record->event.pressed) {
                tap_code16_delay(KC_VOLU, 2);
            }
            return false;
        case TZ_ENC2R:
            if (record->event.pressed) {
                tap_code16_delay(KC_VOLD, 2);
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
}

#define draw_ui_user theme_draw_ui_user
#include "theme_djinn_default.c"
#undef draw_ui_user

#include "diablo3.qgf.c"
#include "avqest12.qff.c"
#include "avqest20.qff.c"

void draw_ui_user(bool force_redraw) {
    uint16_t lcd_width;
    uint16_t lcd_height;
    qp_get_geometry(lcd, &lcd_width, &lcd_height, NULL, NULL, NULL);

    // Redraw a black screen if we've changed typing mode
    static uint32_t last_mode  = 0;
    static bool     logo_drawn = false;
    if (last_mode != typing_mode) {
        last_mode    = typing_mode;
        logo_drawn   = false;
        force_redraw = true;
        qp_rect(lcd, 0, 0, lcd_width - 1, lcd_height - 1, 0, 0, 0, true);
    }

    if (is_keyboard_left() && typing_mode == KC_D3MODE) {
        static const int              x_offsets[4]  = {-90, -30, 30, 90};
        const char                   *time_strs[4]  = {"11.1", "21.5", "39.2", "47.3"};
        static painter_image_handle_t diablo3_logo  = NULL;
        static painter_font_handle_t  avqest12_font = NULL;
        static painter_font_handle_t  avqest20_font = NULL;
        if (!diablo3_logo) {
            diablo3_logo = qp_load_image_mem(gfx_diablo3);
        }
        if (!avqest12_font) {
            avqest12_font = qp_load_font_mem(font_avqest12);
        }
        if (!avqest20_font) {
            avqest20_font = qp_load_font_mem(font_avqest20);
        }

        // Draw the D3 logo if it hasn't been drawn already
        if (!logo_drawn) {
            qp_drawimage(lcd, 0, 0, diablo3_logo);

            qp_circle(lcd, (lcd_width / 2) + x_offsets[0], lcd_height - 1 - avqest20_font->line_height - 30, 26, 0, 0, 255, true);
            qp_circle(lcd, (lcd_width / 2) + x_offsets[1], lcd_height - 1 - avqest20_font->line_height - 30, 26, 0, 0, 255, true);
            qp_circle(lcd, (lcd_width / 2) + x_offsets[2], lcd_height - 1 - avqest20_font->line_height - 30, 26, 0, 0, 255, true);
            qp_circle(lcd, (lcd_width / 2) + x_offsets[3], lcd_height - 1 - avqest20_font->line_height - 30, 26, 0, 0, 255, true);

            qp_drawtext(lcd, (lcd_width / 2) + x_offsets[0] - (qp_textwidth(avqest20_font, "1") / 2), lcd_height - 1 - avqest20_font->line_height, avqest20_font, "1");
            qp_drawtext(lcd, (lcd_width / 2) + x_offsets[1] - (qp_textwidth(avqest20_font, "2") / 2), lcd_height - 1 - avqest20_font->line_height, avqest20_font, "2");
            qp_drawtext(lcd, (lcd_width / 2) + x_offsets[2] - (qp_textwidth(avqest20_font, "3") / 2), lcd_height - 1 - avqest20_font->line_height, avqest20_font, "3");
            qp_drawtext(lcd, (lcd_width / 2) + x_offsets[3] - (qp_textwidth(avqest20_font, "4") / 2), lcd_height - 1 - avqest20_font->line_height, avqest20_font, "4");

            logo_drawn = true;
        }

        // Set up the display items
        static uint32_t last_redraw = 0;
        if (timer_elapsed32(last_redraw) > 25) {
            last_redraw = timer_read32();

            qp_circle(lcd, (lcd_width / 2) + x_offsets[0], lcd_height - 1 - avqest20_font->line_height - 30, 24, 0, 255, 255, true);
            qp_circle(lcd, (lcd_width / 2) + x_offsets[1], lcd_height - 1 - avqest20_font->line_height - 30, 24, 60, 255, 255, true);
            qp_circle(lcd, (lcd_width / 2) + x_offsets[2], lcd_height - 1 - avqest20_font->line_height - 30, 24, 0, 255, 255, true);
            qp_circle(lcd, (lcd_width / 2) + x_offsets[3], lcd_height - 1 - avqest20_font->line_height - 30, 24, 0, 255, 255, true);

            qp_drawtext_recolor(lcd, (lcd_width / 2) + x_offsets[0] - (qp_textwidth(avqest20_font, time_strs[0]) / 2), lcd_height - 1 - avqest20_font->line_height - 30 - (avqest20_font->line_height / 2) + 2, avqest20_font, time_strs[0], 0, 0, 255, 0, 255, 255);
            qp_drawtext_recolor(lcd, (lcd_width / 2) + x_offsets[1] - (qp_textwidth(avqest20_font, time_strs[1]) / 2), lcd_height - 1 - avqest20_font->line_height - 30 - (avqest20_font->line_height / 2) + 2, avqest20_font, time_strs[1], 60, 0, 255, 60, 255, 255);
            qp_drawtext_recolor(lcd, (lcd_width / 2) + x_offsets[2] - (qp_textwidth(avqest20_font, time_strs[2]) / 2), lcd_height - 1 - avqest20_font->line_height - 30 - (avqest20_font->line_height / 2) + 2, avqest20_font, time_strs[2], 0, 0, 255, 0, 255, 255);
            qp_drawtext_recolor(lcd, (lcd_width / 2) + x_offsets[3] - (qp_textwidth(avqest20_font, time_strs[3]) / 2), lcd_height - 1 - avqest20_font->line_height - 30 - (avqest20_font->line_height / 2) + 2, avqest20_font, time_strs[3], 0, 0, 255, 0, 255, 255);
        }
    } else {
        theme_draw_ui_user(force_redraw);
    }

    force_redraw = false;
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
        extern uint8_t prng(void);
        tmp.bytes[0] = prng();
        tmp.bytes[1] = prng();
        tmp.bytes[2] = prng();
        tmp.bytes[3] = prng();

        eeconfig_update_user(tmp.raw);
        uint32_t value = eeconfig_read_user();
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
