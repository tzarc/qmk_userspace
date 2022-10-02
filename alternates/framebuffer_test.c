// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later

//#define TEST_FRAMEBUFFER
#ifdef TEST_FRAMEBUFFER
#    include "thintel15.qff.h"
#    define FRAMEBUFFER_W 240
#    define FRAMEBUFFER_H 120
static uint16_t              surface_buffer[FRAMEBUFFER_W * FRAMEBUFFER_H];
static painter_device_t      surface;
static painter_font_handle_t thintel;
#endif // TEST_FRAMEBUFFER

void keyboard_post_init_keymap(void) {
#ifdef TEST_FRAMEBUFFER
    surface = qp_make_rgb565_surface(FRAMEBUFFER_W, FRAMEBUFFER_H, surface_buffer);
    qp_init(surface, QP_ROTATION_0);
    thintel = qp_load_font_mem(font_thintel15);
#endif // TEST_FRAMEBUFFER
}

void housekeeping_task_keymap(void) {
#ifdef TEST_FRAMEBUFFER
    static uint16_t last_hue = 0xFFFF;
    static uint16_t last_sat = 0xFFFF;
    static uint16_t last_val = 0xFFFF;
    if (last_hue != rgb_matrix_get_hue() || last_sat != rgb_matrix_get_sat() || last_val != rgb_matrix_get_val()) {
        last_hue = rgb_matrix_get_hue();
        last_sat = rgb_matrix_get_sat();
        last_val = rgb_matrix_get_val();

        // All bars are inset by 20px on each side, 200px wide, 80px left for bars
        // All bars are vertically spaced by 10px, 20px high each

        // Start with a black background
        qp_rect(surface, 0, 0, FRAMEBUFFER_W - 1, FRAMEBUFFER_H - 1, 0, 0, 0, true);

        // Draw hue
        qp_line(surface, 0, 17, FRAMEBUFFER_W - 1, 17, 0, 0, 96);
        qp_rect(surface, 0, 18, FRAMEBUFFER_W - 1, 47, 0, 0, 32, true);
        qp_line(surface, 0, 48, FRAMEBUFFER_W - 1, 48, 0, 0, 96);
        for (uint8_t i = 0; i < 200; ++i) {
            qp_line(surface, 20 + i, 20, 20 + i, 40, ((uint16_t)i * 255) / 200, 255, 255);
            q
        }
        uint16_t xpos = (last_hue * 200) / 255;
        for (uint8_t i = 0; i < 6; ++i) {
            qp_line(surface, 20 + xpos - i, 41 + i, 20 + xpos + i, 41 + i, 255, 0, 255);
        }

        // Draw saturation
        for (uint8_t i = 0; i < 200; ++i) {
            qp_line(surface, 20 + i, 50, 20 + i, 70, last_hue, ((uint16_t)i * 255) / 200, last_val);
        }
        xpos = (last_sat * 200) / 255;
        for (uint8_t i = 0; i < 6; ++i) {
            qp_line(surface, 20 + xpos - i, 71 + i, 20 + xpos + i, 71 + i, 255, 0, 255);
        }

        // Draw value
        for (uint8_t i = 0; i < 200; ++i) {
            qp_line(surface, 20 + i, 80, 20 + i, 100, last_hue, last_sat, ((uint16_t)i * 255) / 200);
        }
        xpos = (last_val * 200) / 255;
        for (uint8_t i = 0; i < 6; ++i) {
            qp_line(surface, 20 + xpos - i, 101 + i, 20 + xpos + i, 101 + i, 255, 0, 255);
        }

        qp_rgb565_surface_draw(surface, lcd, 0, 0);
        qp_flush(lcd);
    }
#endif // TEST_FRAMEBUFFER
}
