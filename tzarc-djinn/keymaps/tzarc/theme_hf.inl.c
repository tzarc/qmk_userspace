
#include "graphics/src/hf_bg_left.c"
#include "graphics/src/hf_bg_right.c"
#include "graphics/src/hf_lock_caps_ON.c"
#include "graphics/src/hf_lock_caps_OFF.c"
#include "graphics/src/hf_lock_num_ON.c"
#include "graphics/src/hf_lock_num_OFF.c"
#include "graphics/src/hf_lock_scrl_ON.c"
#include "graphics/src/hf_lock_scrl_OFF.c"
#include "graphics/src/thintel15.c"

void draw_ui_user(void) {
    bool            layer_state_redraw = false;
    static uint32_t last_layer_state   = 0;
    if (last_layer_state != layer_state) {
        last_layer_state   = layer_state;
        layer_state_redraw = true;
    }

    bool                     power_state_redraw = false;
    static usbpd_allowance_t last_current_state = (usbpd_allowance_t)(~0);
    if (last_current_state != kb_state.current_setting) {
        last_current_state = kb_state.current_setting;
        power_state_redraw = true;
    }

    bool            scan_redraw      = false;
    static uint32_t last_scan_update = 0;
    if (timer_elapsed32(last_scan_update) > 125) {
        last_scan_update = timer_read32();
        scan_redraw      = true;
    }

    bool            wpm_redraw      = false;
    static uint32_t last_wpm_update = 0;
    if (timer_elapsed32(last_wpm_update) > 125) {
        last_wpm_update = timer_read32();
        wpm_redraw      = true;
    }

#if defined(RGB_MATRIX_ENABLE)
    bool            rgb_effect_redraw = false;
    static uint16_t last_effect       = 0xFFFF;
    uint8_t         curr_effect       = rgb_matrix_config.mode;
    if (last_effect != curr_effect) {
        last_effect       = curr_effect;
        rgb_effect_redraw = true;
    }
#endif  // defined(RGB_MATRIX_ENABLE)

    // Show the Djinn logo and two vertical bars on both sides
    static bool first_draw_complete = false;
    if (!first_draw_complete) {
        if (is_keyboard_left()) {
            qp_drawimage(lcd, 0, 0, gfx_hf_bg_left);
        } else {
            qp_drawimage(lcd, 0, 0, gfx_hf_bg_right);
        }
    }

    int initial_xpos = 8;
    int ypadding     = 8;
    int ypos         = 319 - (font_thintel15->glyph_height + ypadding);

    uint8_t text_fg_hue = (180 * 255) / 360;
    uint8_t text_fg_sat = 0;
    uint8_t text_bg_hue = (272 * 255) / 360;

    // Show layer info on the left side
    if (is_keyboard_left()) {
        char buf[32] = {0};
        int  xpos    = initial_xpos;

#if defined(RGB_MATRIX_ENABLE)
        if (!first_draw_complete || rgb_effect_redraw) {
            static int max_rgb_xpos = 0;
            xpos                    = initial_xpos;
            snprintf_(buf, sizeof(buf), "rgb: %s", rgb_matrix_name(curr_effect));

            for (int i = 5; i < sizeof(buf); ++i) {
                if (buf[i] == 0)
                    break;
                else if (buf[i] == '_')
                    buf[i] = ' ';
                else if (buf[i - 1] == ' ')
                    buf[i] = toupper(buf[i]);
                else if (buf[i - 1] != ' ')
                    buf[i] = tolower(buf[i]);
            }

            xpos += qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, text_fg_hue, text_fg_sat, 255, text_bg_hue, (98 * 255) / 100, 255);
            if (max_rgb_xpos < xpos) {
                max_rgb_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_rgb_xpos, ypos + font_thintel15->glyph_height, text_bg_hue, (98 * 255) / 100, 255, true);
        }

        ypos -= font_thintel15->glyph_height + ypadding;
#endif  // defined(RGB_MATRIX_ENABLE)

        if (!first_draw_complete || layer_state_redraw) {
            const char *layer_name = "unknown";
            switch (get_highest_layer(layer_state)) {
                case LAYER_BASE:
                    layer_name = "qwerty";
                    break;
                case LAYER_LOWER:
                    layer_name = "lower";
                    break;
                case LAYER_RAISE:
                    layer_name = "raise";
                    break;
                case LAYER_ADJUST:
                    layer_name = "adjust";
                    break;
            }

            static int max_layer_xpos = 0;
            xpos                      = initial_xpos;
            snprintf_(buf, sizeof(buf), "layer: %s", layer_name);
            xpos += qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, text_fg_hue, text_fg_sat, 255, text_bg_hue, (98 * 255) / 100, 255);
            if (max_layer_xpos < xpos) {
                max_layer_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_layer_xpos, ypos + font_thintel15->glyph_height, text_bg_hue, (98 * 255) / 100, 255, true);
        }

        ypos -= font_thintel15->glyph_height + ypadding;

        if (!first_draw_complete || power_state_redraw) {
            static int max_power_xpos = 0;
            xpos                      = initial_xpos;
            snprintf_(buf, sizeof(buf), "power: %s", usbpd_str(kb_state.current_setting));
            xpos += qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, text_fg_hue, text_fg_sat, 255, text_bg_hue, (98 * 255) / 100, 255);
            if (max_power_xpos < xpos) {
                max_power_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_power_xpos, ypos + font_thintel15->glyph_height, text_bg_hue, (98 * 255) / 100, 255, true);
        }

        ypos -= font_thintel15->glyph_height + ypadding;

        if (!first_draw_complete || scan_redraw) {
            static int max_scans_xpos = 0;
            xpos                      = initial_xpos;
            snprintf_(buf, sizeof(buf), "scans: %d", (int)user_state.scan_rate);
            xpos += qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, text_fg_hue, text_fg_sat, 255, text_bg_hue, (98 * 255) / 100, 255);
            if (max_scans_xpos < xpos) {
                max_scans_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_scans_xpos, ypos + font_thintel15->glyph_height, text_bg_hue, (98 * 255) / 100, 255, true);
        }

        ypos -= font_thintel15->glyph_height + 6;

        if (!first_draw_complete || wpm_redraw) {
            static int max_wpm_xpos = 0;
            xpos                    = initial_xpos;
            snprintf_(buf, sizeof(buf), "wpm: %d", (int)get_current_wpm());
            xpos += qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, text_fg_hue, text_fg_sat, 255, text_bg_hue, (98 * 255) / 100, 255);
            if (max_wpm_xpos < xpos) {
                max_wpm_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_wpm_xpos, ypos + font_thintel15->glyph_height, text_bg_hue, (98 * 255) / 100, 255, true);
        }

        ypos -= font_thintel15->glyph_height + ypadding;
    }

    // Show LED lock indicators on the right side
    if (!is_keyboard_left()) {
        const int    offset         = 24;
        static led_t last_led_state = {0};
        if (!first_draw_complete || last_led_state.raw != host_keyboard_led_state().raw) {
            last_led_state.raw = host_keyboard_led_state().raw;
            qp_drawimage(lcd, 239 - offset - 48, 319 - ((48 + offset) * 3), last_led_state.caps_lock ? gfx_hf_lock_caps_ON : gfx_hf_lock_caps_OFF);
            qp_drawimage(lcd, 239 - offset - 48, 319 - ((48 + offset) * 2), last_led_state.num_lock ? gfx_hf_lock_num_ON : gfx_hf_lock_num_OFF);
            qp_drawimage(lcd, 239 - offset - 48, 319 - ((48 + offset) * 1), last_led_state.scroll_lock ? gfx_hf_lock_scrl_ON : gfx_hf_lock_scrl_OFF);
        }
    }

    first_draw_complete = true;
}
