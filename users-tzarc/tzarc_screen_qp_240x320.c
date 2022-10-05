// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#include <qp.h>
#include "tzarc.h"
#include "diablo3.qgf.c"
#include "avqest12.qff.c"
#include "avqest20.qff.c"

void draw_screen_diablo3(bool force_redraw) {
    uint16_t display_width;
    uint16_t display_height;
    qp_get_geometry(display_panel, &display_width, &display_height, NULL, NULL, NULL);

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
    if (force_redraw) {
        qp_drawimage(display_panel, 0, 0, diablo3_logo);

        qp_circle(display_panel, (display_width / 2) + x_offsets[0], display_height - 1 - avqest20_font->line_height - 30, 26, 0, 0, 255, true);
        qp_circle(display_panel, (display_width / 2) + x_offsets[1], display_height - 1 - avqest20_font->line_height - 30, 26, 0, 0, 255, true);
        qp_circle(display_panel, (display_width / 2) + x_offsets[2], display_height - 1 - avqest20_font->line_height - 30, 26, 0, 0, 255, true);
        qp_circle(display_panel, (display_width / 2) + x_offsets[3], display_height - 1 - avqest20_font->line_height - 30, 26, 0, 0, 255, true);

        qp_drawtext(display_panel, (display_width / 2) + x_offsets[0] - (qp_textwidth(avqest20_font, "1") / 2), display_height - 1 - avqest20_font->line_height, avqest20_font, "1");
        qp_drawtext(display_panel, (display_width / 2) + x_offsets[1] - (qp_textwidth(avqest20_font, "2") / 2), display_height - 1 - avqest20_font->line_height, avqest20_font, "2");
        qp_drawtext(display_panel, (display_width / 2) + x_offsets[2] - (qp_textwidth(avqest20_font, "3") / 2), display_height - 1 - avqest20_font->line_height, avqest20_font, "3");
        qp_drawtext(display_panel, (display_width / 2) + x_offsets[3] - (qp_textwidth(avqest20_font, "4") / 2), display_height - 1 - avqest20_font->line_height, avqest20_font, "4");
    }

    // Set up the display items
    static uint32_t last_redraw = 0;
    if (timer_elapsed32(last_redraw) > 25) {
        last_redraw = timer_read32();

        qp_circle(display_panel, (display_width / 2) + x_offsets[0], display_height - 1 - avqest20_font->line_height - 30, 24, 0, 255, 255, true);
        qp_circle(display_panel, (display_width / 2) + x_offsets[1], display_height - 1 - avqest20_font->line_height - 30, 24, 60, 255, 255, true);
        qp_circle(display_panel, (display_width / 2) + x_offsets[2], display_height - 1 - avqest20_font->line_height - 30, 24, 0, 255, 255, true);
        qp_circle(display_panel, (display_width / 2) + x_offsets[3], display_height - 1 - avqest20_font->line_height - 30, 24, 0, 255, 255, true);

        qp_drawtext_recolor(display_panel, (display_width / 2) + x_offsets[0] - (qp_textwidth(avqest20_font, time_strs[0]) / 2), display_height - 1 - avqest20_font->line_height - 30 - (avqest20_font->line_height / 2) + 2, avqest20_font, time_strs[0], 0, 0, 255, 0, 255, 255);
        qp_drawtext_recolor(display_panel, (display_width / 2) + x_offsets[1] - (qp_textwidth(avqest20_font, time_strs[1]) / 2), display_height - 1 - avqest20_font->line_height - 30 - (avqest20_font->line_height / 2) + 2, avqest20_font, time_strs[1], 60, 0, 255, 60, 255, 255);
        qp_drawtext_recolor(display_panel, (display_width / 2) + x_offsets[2] - (qp_textwidth(avqest20_font, time_strs[2]) / 2), display_height - 1 - avqest20_font->line_height - 30 - (avqest20_font->line_height / 2) + 2, avqest20_font, time_strs[2], 0, 0, 255, 0, 255, 255);
        qp_drawtext_recolor(display_panel, (display_width / 2) + x_offsets[3] - (qp_textwidth(avqest20_font, time_strs[3]) / 2), display_height - 1 - avqest20_font->line_height - 30 - (avqest20_font->line_height / 2) + 2, avqest20_font, time_strs[3], 0, 0, 255, 0, 255, 255);
    }
}

void draw_screen(bool force_redraw) {
    uint16_t display_width;
    uint16_t display_height;
    qp_get_geometry(display_panel, &display_width, &display_height, NULL, NULL, NULL);

    // Redraw a black screen if we've changed typing mode
    static typing_mode_t last_mode = (typing_mode_t)(~0);
    if (last_mode != typing_mode) {
        last_mode    = typing_mode;
        force_redraw = true;
        qp_rect(display_panel, 0, 0, display_width - 1, display_height - 1, 0, 0, 0, true);
    }

    if (is_keyboard_left() && typing_mode == MODE_D3) {
        draw_screen_diablo3(force_redraw);
    } else {
        draw_screen_base(force_redraw);
    }

    force_redraw = false;
}
