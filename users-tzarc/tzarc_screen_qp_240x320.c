// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#include <qp.h>
#include "tzarc.h"
#include "diablo3.qgf.c"
#include "avqest12.qff.c"
#include "avqest20.qff.c"
#include "thintel15.qff.h"
#include "everex_5x8.qff.c"

static painter_font_handle_t thintel;
static painter_font_handle_t everex_5x8;

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

#define NUM_LOG_LINES 10
#define MAX_LOG_LINE 43

static bool    needs_redraw  = false;
static uint8_t log_write_idx = 0;
static char    loglines[NUM_LOG_LINES][MAX_LOG_LINE + 2];
static char   *logline_ptrs[NUM_LOG_LINES];

void tzarc_sendchar_hook(uint8_t c) {
    static bool first_setup = false;
    if (!first_setup) {
        memset(loglines, 0, sizeof(loglines));
        for (int i = 0; i < NUM_LOG_LINES; ++i) {
            logline_ptrs[i] = loglines[i];
        }
        first_setup = true;
    }

    if (c == '\n') {
        logline_ptrs[NUM_LOG_LINES - 1][log_write_idx] = 0;
        char *tmp                                      = logline_ptrs[0];
        for (int i = 0; i < NUM_LOG_LINES - 1; ++i) {
            logline_ptrs[i] = logline_ptrs[i + 1];
        }
        logline_ptrs[NUM_LOG_LINES - 1]    = tmp;
        log_write_idx                      = 0;
        logline_ptrs[NUM_LOG_LINES - 1][0] = 0;
        needs_redraw                       = true;
    } else if (log_write_idx >= (MAX_LOG_LINE)) {
        // Ignore.
    } else {
        logline_ptrs[NUM_LOG_LINES - 1][log_write_idx++] = c;
        logline_ptrs[NUM_LOG_LINES - 1][log_write_idx]   = 0;
        needs_redraw                                     = true;
    }
}

void draw_screen(bool force_redraw) {
    uint16_t display_width;
    uint16_t display_height;
    qp_get_geometry(display_panel, &display_width, &display_height, NULL, NULL, NULL);

    if (!thintel) {
        thintel = qp_load_font_mem(font_thintel15);
    }
    if (!everex_5x8) {
        everex_5x8 = qp_load_font_mem(font_everex_5x8);
    }

    // Redraw a black screen if we've changed typing mode
    static typing_mode_t last_mode   = (typing_mode_t)(~0);
    bool                 redraw_mode = false;
    if (last_mode != typing_mode) {
        // Check if we need to force-redraw due to full screen mode change
        if (last_mode == MODE_D3 || typing_mode == MODE_D3) {
            force_redraw = true;
        }

        last_mode   = typing_mode;
        redraw_mode = true;
    }

    if (force_redraw) {
        qp_rect(display_panel, 0, 0, display_width - 1, display_height - 1, 0, 0, 0, true);
    }

    if (is_keyboard_left() && typing_mode == MODE_D3) {
        draw_screen_diablo3(force_redraw);
    } else {
        draw_screen_base(force_redraw);

        int ypos = 4;

        // Redraw typing mode if required
        char            buf[64]  = {0};
        static uint16_t last_hue = 0xFFFF;
        uint16_t        curr_hue = rgb_matrix_get_hue();
        if (last_hue != curr_hue || force_redraw || redraw_mode) {
            static int max_mode_w = 0;
            snprintf(buf, sizeof(buf), "mode: %s", typing_mode_name(typing_mode));

            int w = qp_textwidth(thintel, buf);
            if (max_mode_w < w) {
                max_mode_w = w;
            }

            qp_drawtext_recolor(display_panel, display_width - 16 - w, ypos, thintel, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            qp_rect(display_panel, display_width - 1 - 16 - w, ypos, display_width - 1 - 16 - max_mode_w - 1, ypos + thintel->line_height, 0, 0, 0, true);
        }

        ypos += thintel->line_height + 4;

        static uint8_t last_unicode_mode = 0xFF;
        uint8_t        curr_unicode_mode = get_unicode_input_mode();
        if (last_hue != curr_hue || last_unicode_mode != curr_unicode_mode || force_redraw) {
            last_unicode_mode = curr_unicode_mode;

            static int max_unicode_w = 0;
            snprintf(buf, sizeof(buf), "unicode: %s", unicode_mode_name(curr_unicode_mode));

            int w = qp_textwidth(thintel, buf);
            if (max_unicode_w < w) {
                max_unicode_w = w;
            }

            qp_drawtext_recolor(display_panel, display_width - 16 - w, ypos, thintel, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            qp_rect(display_panel, display_width - 1 - 16 - w, ypos, display_width - 1 - 16 - max_unicode_w - 1, ypos + thintel->line_height, 0, 0, 0, true);
        }

        static uint32_t last_log_redraw = 0;
        if (last_hue != curr_hue || force_redraw || (needs_redraw && timer_elapsed32(last_log_redraw) > 50)) {
            static int16_t max_line_width = 0;
            for (int i = 0; i < NUM_LOG_LINES - 1; ++i) {
                int16_t line_width = qp_drawtext_recolor(display_panel, 12, display_height - (NUM_LOG_LINES - i - 1) * everex_5x8->line_height, everex_5x8, logline_ptrs[i], curr_hue, 255, 255, curr_hue, 255, 0);
                if (max_line_width <= line_width) {
                    max_line_width = line_width;
                } else {
                    qp_rect(display_panel, 12 + line_width, display_height - (NUM_LOG_LINES - i - 1) * everex_5x8->line_height, 12 + max_line_width, display_height - (NUM_LOG_LINES - i - 2) * everex_5x8->line_height, 0, 0, 0, true);
                }
            }
            last_log_redraw = timer_read32();
            needs_redraw    = false;
        }

        last_hue = curr_hue;
    }
}
