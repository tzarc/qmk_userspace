
#include "graphics/src/djinn.c"
#include "graphics/src/lock-caps-ON.c"
#include "graphics/src/lock-scrl-ON.c"
#include "graphics/src/lock-num-ON.c"
#include "graphics/src/lock-caps-OFF.c"
#include "graphics/src/lock-scrl-OFF.c"
#include "graphics/src/lock-num-OFF.c"
#include "graphics/src/thintel15.c"

void draw_ui_user(void) {
    bool            hue_redraw = false;
    static uint16_t last_hue   = 0xFFFF;
    uint8_t         curr_hue   = rgblight_get_hue();
    if (last_hue != curr_hue) {
        last_hue   = curr_hue;
        hue_redraw = true;
    }

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
    if (hue_redraw) {
        qp_drawimage_recolor(lcd, 120 - gfx_djinn->width / 2, 32, gfx_djinn, curr_hue, 255, 255);
        qp_rect(lcd, 0, 0, 8, 319, curr_hue, 255, 255, true);
        qp_rect(lcd, 231, 0, 239, 319, curr_hue, 255, 255, false);

        qp_line(lcd, 200 - 8, 0, 239 - 8, 39, curr_hue, 255, 255);
        qp_line(lcd, 200 - 8, 0, 200 - 8, 39, curr_hue, 255, 255);
        qp_line(lcd, 200 - 8, 39, 239 - 8, 39, curr_hue, 255, 255);
        qp_circle(lcd, 205 - 8, 5, 4, curr_hue, 255, 255, true);
        qp_circle(lcd, 234 - 8, 34, 4, curr_hue, 255, 255, false);

        qp_ellipse(lcd, 214 - 8, 14, 4, 8, curr_hue, 255, 255, false);
        qp_ellipse(lcd, 225 - 8, 25, 8, 4, curr_hue, 255, 255, true);
    }

    int ypos = 4;

    // Show layer info on the left side
    if (is_keyboard_left()) {
        char buf[32] = {0};
        int  xpos    = 16;

#if defined(RGB_MATRIX_ENABLE)
        if (hue_redraw || rgb_effect_redraw) {
            static int max_rgb_xpos = 0;
            xpos                    = 16;
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

            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_rgb_xpos < xpos) {
                max_rgb_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_rgb_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);
        }

        ypos += font_thintel15->glyph_height + 4;
#endif  // defined(RGB_MATRIX_ENABLE)

        if (hue_redraw || layer_state_redraw) {
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
            xpos                      = 16;
            snprintf_(buf, sizeof(buf), "layer: %s", layer_name);
            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_layer_xpos < xpos) {
                max_layer_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_layer_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);
        }

        ypos += font_thintel15->glyph_height + 4;

        if (hue_redraw || power_state_redraw) {
            static int max_power_xpos = 0;
            xpos                      = 16;
            snprintf_(buf, sizeof(buf), "power: %s", usbpd_str(kb_state.current_setting));
            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_power_xpos < xpos) {
                max_power_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_power_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);
        }

        ypos += font_thintel15->glyph_height + 4;

        if (hue_redraw || scan_redraw) {
            static int max_scans_xpos = 0;
            xpos                      = 16;
            snprintf_(buf, sizeof(buf), "scans: %d", (int)user_state.scan_rate);
            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_scans_xpos < xpos) {
                max_scans_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_scans_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);
        }

        ypos += font_thintel15->glyph_height + 4;

        if (hue_redraw || wpm_redraw) {
            static int max_wpm_xpos = 0;
            xpos                    = 16;
            snprintf_(buf, sizeof(buf), "wpm: %d", (int)get_current_wpm());
            xpos = qp_drawtext_recolor(lcd, xpos, ypos, font_thintel15, buf, curr_hue, 255, 255, curr_hue, 255, 0);
            if (max_wpm_xpos < xpos) {
                max_wpm_xpos = xpos;
            }
            qp_rect(lcd, xpos, ypos, max_wpm_xpos, ypos + font_thintel15->glyph_height, 0, 0, 0, true);
        }

        ypos += font_thintel15->glyph_height + 4;
    }

    // Show LED lock indicators on the right side
    if (!is_keyboard_left()) {
        static led_t last_led_state = {0};
        if (hue_redraw || last_led_state.raw != host_keyboard_led_state().raw) {
            last_led_state.raw = host_keyboard_led_state().raw;
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 3), 0, last_led_state.caps_lock ? gfx_lock_caps_ON : gfx_lock_caps_OFF, curr_hue, 255, last_led_state.caps_lock ? 255 : 32);
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 2), 0, last_led_state.num_lock ? gfx_lock_num_ON : gfx_lock_num_OFF, curr_hue, 255, last_led_state.num_lock ? 255 : 32);
            qp_drawimage_recolor(lcd, 239 - 12 - (32 * 1), 0, last_led_state.scroll_lock ? gfx_lock_scrl_ON : gfx_lock_scrl_OFF, curr_hue, 255, last_led_state.scroll_lock ? 255 : 32);
        }
    }

#if 0
    // Test code
    if (redraw_required) {
        qp_line(lcd, 8, 0, 32, 319, curr_hue, 255, 255);
        qp_line(lcd, 32, 0, 8, 319, curr_hue, 255, 255);
        for (int i = 0; i < 8; ++i) qp_circle(lcd, 20, (i * 40) + 20, 10, curr_hue, 255, 255, (i % 2) == 0);
        qp_drawtext(lcd, 0, 0, font_noto16, "So this is a test of font rendering");
        qp_drawtext_recolor(lcd, 0, font_noto16->glyph_height, font_noto16, "with Quantum Painter...", 0, 255, 255, 0, 255, 0);
        qp_drawtext_recolor(lcd, 0, 2 * font_noto16->glyph_height, font_noto16, "Perhaps a different background?", 43, 255, 255, 169, 255, 255);
        qp_drawtext(lcd, 0, 3 * font_noto16->glyph_height, font_noto28, "Unicode: ĄȽɂɻɣɈʣ");
        qp_drawtext(lcd, 0, 3 * font_noto16->glyph_height + font_noto28->glyph_height, font_thintel15, "And here we are, with another font!");
    }
#endif
}