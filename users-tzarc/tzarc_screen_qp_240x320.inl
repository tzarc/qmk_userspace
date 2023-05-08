// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <ctype.h>
#include <qp.h>
#include "painter/generic/qp_surface.h"
#include "qp_internal_formats.h"
#include "quantum_keycodes.h"
#include "tzarc.h"
#include "diablo.qgf.c"
#include "exocet.qff.c"
#include "thintel15.qff.h"
#include "everex_5x8.qff.c"

#define DIABLO_SURFACE_WIDTH 240
#define DIABLO_SURFACE_HEIGHT 80
static uint8_t               diablo_framebuffer[SURFACE_REQUIRED_BUFFER_BYTE_SIZE(DIABLO_SURFACE_WIDTH, DIABLO_SURFACE_HEIGHT, 16)];
static painter_device_t      diablo_surface = NULL;
static painter_font_handle_t thintel        = NULL;
static painter_font_handle_t everex_5x8     = NULL;

void draw_screen_diablo(bool force_redraw) {
    static uint16_t display_width;
    static uint16_t display_height;
    static bool     geometry_retrieved = false;
    if (!geometry_retrieved) {
        qp_get_geometry(display_panel, &display_width, &display_height, NULL, NULL, NULL);
        geometry_retrieved = true;
    }

    static const int              x_offsets[DIABLO_NUM_KEYS] = {-90, -45, 0, 45, 90};
    static painter_image_handle_t diablo_logo                = NULL;
    static painter_font_handle_t  exocet_font                = NULL;
    if (!diablo_surface) {
        diablo_surface = qp_make_rgb565_surface(DIABLO_SURFACE_WIDTH, DIABLO_SURFACE_HEIGHT, diablo_framebuffer);
        qp_init(diablo_surface, QP_ROTATION_0);
    }
    if (!diablo_logo) {
        diablo_logo = qp_load_image_mem(gfx_diablo);
    }
    if (!exocet_font) {
        exocet_font = qp_load_font_mem(font_exocet);
    }

    // Draw the Diablo logo if it hasn't been drawn already
    if (force_redraw) {
        qp_drawimage(display_panel, 0, 0, diablo_logo);
    }

    // Set up the display items
    static struct tzarc_eeprom_cfg_t last_eeprom_cfg     = {0};
    static struct diablo_runtime_t   last_diablo_runtime = {0};
    static bool                      last_active_state   = false;
    bool                             active_state        = diablo_automatic_active();
    bool                             should_redraw       = force_redraw                                  // If we want to force
                         || last_active_state != active_state                                            // If the active state changed
                         || (memcmp(&last_eeprom_cfg, &tzarc_eeprom_cfg, sizeof(tzarc_eeprom_cfg)) != 0) // If the eeprom state changed
                         || (memcmp(&diablo_runtime, &last_diablo_runtime, sizeof(diablo_runtime)) != 0) // If the runtime state changed
        ;

    if (should_redraw) {
        // Save last state
        last_active_state = active_state;
        memcpy(&last_eeprom_cfg, &tzarc_eeprom_cfg, sizeof(tzarc_eeprom_cfg));
        memcpy(&last_diablo_runtime, &diablo_runtime, sizeof(diablo_runtime));

        // Clear the on/off or config marker line
        qp_rect(diablo_surface, 0, DIABLO_SURFACE_HEIGHT - 30, DIABLO_SURFACE_WIDTH - 1, DIABLO_SURFACE_HEIGHT - 1, 0, 0, 0, true);

        if (diablo_runtime.config_mode_active) {
            // Draw the selection marker
            qp_circle(diablo_surface, (DIABLO_SURFACE_WIDTH / 2) + x_offsets[diablo_runtime.config_curr_selection], DIABLO_SURFACE_HEIGHT - 12, 8, 0, 0, 255, true);
        } else {
            // Draw the on/off state
            uint16_t hue = diablo_automatic_active() ? 80 : 0;

            qp_circle(diablo_surface, 30, DIABLO_SURFACE_HEIGHT - 12, 10, hue, 255, 160, true);
            qp_circle(diablo_surface, DIABLO_SURFACE_WIDTH - 1 - 30, DIABLO_SURFACE_HEIGHT - 12, 10, hue, 255, 160, true);
            qp_rect(diablo_surface, 30, DIABLO_SURFACE_HEIGHT - 12 - 10, DIABLO_SURFACE_WIDTH - 1 - 30, DIABLO_SURFACE_HEIGHT - 12 + 10, hue, 255, 160, true);

            qp_circle(diablo_surface, 30, DIABLO_SURFACE_HEIGHT - 12, 8, hue, 255, 80, true);
            qp_circle(diablo_surface, DIABLO_SURFACE_WIDTH - 1 - 30, DIABLO_SURFACE_HEIGHT - 12, 8, hue, 255, 80, true);
            qp_rect(diablo_surface, 30, DIABLO_SURFACE_HEIGHT - 12 - 8, DIABLO_SURFACE_WIDTH - 1 - 30, DIABLO_SURFACE_HEIGHT - 12 + 8, hue, 255, 80, true);
        }

        // Draw the enabled circles
        for (int i = 0; i < DIABLO_NUM_KEYS; ++i) {
            uint16_t keycode = diablo_index_to_keycode(i);
            char     str[16];
            strncpy(str, key_name(keycode, false), sizeof(str) - 1);
            str[0]             = toupper(str[0]);
            bool       enabled = diablo_automatic_key_enabled(keycode);
            bool       pressed = diablo_runtime.key_desc[i].pressed;
            qp_pixel_t ol, fg, bg;
            if (pressed) {
                ol = (qp_pixel_t){.hsv888 = {.h = 80, .s = 0, .v = 255}};
                fg = (qp_pixel_t){.hsv888 = {.h = 80, .s = 0, .v = 255}};
                bg = (qp_pixel_t){.hsv888 = {.h = 80, .s = 255, .v = 160}};
            } else if (enabled) {
                ol = (qp_pixel_t){.hsv888 = {.h = 80, .s = 255, .v = 160}};
                fg = (qp_pixel_t){.hsv888 = {.h = 80, .s = 0, .v = 255}};
                bg = (qp_pixel_t){.hsv888 = {.h = 80, .s = 255, .v = 30}};
            } else {
                ol = (qp_pixel_t){.hsv888 = {.h = 0, .s = 255, .v = 160}};
                fg = (qp_pixel_t){.hsv888 = {.h = 0, .s = 0, .v = 255}};
                bg = (qp_pixel_t){.hsv888 = {.h = 0, .s = 255, .v = 30}};
            }

#define UNPACK_HSV(col) (col).hsv888.h, (col).hsv888.s, (col).hsv888.v

            // Outline
            qp_circle(diablo_surface, (DIABLO_SURFACE_WIDTH / 2) + x_offsets[i], DIABLO_SURFACE_HEIGHT - 1 - exocet_font->line_height - 21, 22, UNPACK_HSV(ol), true);
            // Inner
            qp_circle(diablo_surface, (DIABLO_SURFACE_WIDTH / 2) + x_offsets[i], DIABLO_SURFACE_HEIGHT - 1 - exocet_font->line_height - 21, 20, UNPACK_HSV(bg), true);
            // Text
            qp_drawtext_recolor(diablo_surface, (DIABLO_SURFACE_WIDTH / 2) + x_offsets[i] - (qp_textwidth(exocet_font, str) / 2), DIABLO_SURFACE_HEIGHT - 1 - exocet_font->line_height - 21 - (exocet_font->line_height / 2) + 2, exocet_font, str, UNPACK_HSV(fg), UNPACK_HSV(bg));
        }

        // Copy the surface to the display
        qp_surface_draw(diablo_surface, display_panel, 0, display_height - DIABLO_SURFACE_HEIGHT, force_redraw);
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
    static uint16_t display_width;
    static uint16_t display_height;
    static bool     geometry_retrieved = false;
    if (!geometry_retrieved) {
        qp_get_geometry(display_panel, &display_width, &display_height, NULL, NULL, NULL);
        geometry_retrieved = true;
    }

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
        if (last_mode == MODE_DIABLO || typing_mode == MODE_DIABLO) {
            force_redraw = true;
        }

        last_mode   = typing_mode;
        redraw_mode = true;
    }

    if (force_redraw) {
        qp_rect(display_panel, 0, 0, display_width - 1, display_height - 1, 0, 0, 0, true);
    }

    if (is_keyboard_left() && typing_mode == MODE_DIABLO) {
        draw_screen_diablo(force_redraw);
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
