// Copyright 2021 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <quantum.h>
#include <debug.h>
#include <qp.h>
#include <qp_ili9163.h>
#include <qp_st7789.h>
#include <qp_ssd1351.h>

// Test assets
#include "graphics/test-image.qgf.c"
#include "graphics/test-anim.qgf.c"
#include "graphics/loading.qgf.c"
#include "graphics/thintel.qff.c"

painter_device_t ili9163;
painter_device_t st7789;
painter_device_t ssd1351;

painter_image_handle_t test_image;
painter_image_handle_t test_anim;
painter_image_handle_t loading;

painter_font_handle_t thintel;

static uint32_t delayed_test(uint32_t trigger_time, void *cb_arg) {
    uint16_t        timeout = (uint16_t)(uintptr_t)cb_arg;
    static uint32_t last    = 0;
    uint32_t        now     = timer_read32();

    if (!thintel) {
        thintel = qp_load_font_mem(font_thintel);
    }

    dprintf("AHOY THERE MATEY @ %d: %d, delta = %d, font = %s\n", (int)timeout, (int)now, (int)(now - last), thintel != NULL ? "loaded" : "failed");

    last = now;
    return timeout;
}

void init_and_clear(painter_device_t device, painter_rotation_t rotation) {
    uint16_t width;
    uint16_t height;
    qp_get_geometry(device, &width, &height, NULL, NULL, NULL);

    qp_init(device, rotation);
    qp_rect(device, 0, 0, width - 1, height - 1, 0, 0, 0, true);
}

void draw_test(painter_device_t device, const char *name, uint32_t now) {
    uint16_t width;
    uint16_t height;
    qp_get_geometry(device, &width, &height, NULL, NULL, NULL);

    static uint8_t hue = 0;
    hue += 4;

    char buf1[32] = {0};
    char buf2[32] = {0};
    char buf3[32] = {0};
    sprintf(buf1, "QMK on %s!", name);
    sprintf(buf2, "MCU time is: %dms", (int)now);
    sprintf(buf3, "Hue is: %d", (int)hue);

    if (test_image) {
        qp_drawimage_recolor(device, 0, 128 - test_image->height, test_image, hue, 255, 255, hue, 255, 0);
    }

    int16_t xpos = 0;
    xpos         = qp_drawtext(device, 0, 128 - 3 * thintel->line_height, thintel, buf1);
    qp_rect(device, xpos, 128 - 3 * thintel->line_height, width - 1, 127 - 2 * thintel->line_height, 0, 0, 0, true);
    xpos = qp_drawtext_recolor(device, 0, 128 - 2 * thintel->line_height, thintel, buf2, hue, 255, 255, hue, 255, 0);
    qp_rect(device, xpos, 128 - 2 * thintel->line_height, width - 1, 127 - 1 * thintel->line_height, 0, 0, 0, true);
    xpos = qp_drawtext_recolor(device, 0, 128 - 1 * thintel->line_height, thintel, buf3, hue, 255, 255, hue, 255, 0);
    qp_rect(device, xpos, 128 - 1 * thintel->line_height, width - 1, 127 - 0 * thintel->line_height, 0, 0, 0, true);

    qp_flush(device);
}

void keyboard_post_init_kb(void) {
    debug_enable   = true;
    debug_matrix   = true;
    debug_keyboard = true;

    defer_exec(3000, delayed_test, (void *)(uint16_t)3000);
    defer_exec(2900, delayed_test, (void *)(uint16_t)2900);

    test_image = qp_load_image_mem(gfx_test_image);
    test_anim  = qp_load_image_mem(gfx_test_anim);
    loading    = qp_load_image_mem(gfx_loading);

    thintel = qp_load_font_mem(font_thintel);

    ili9163 = qp_ili9163_make_spi_device(128, 128, DISPLAY_CS_PIN_1_44_INCH_LCD_ILI9163, DISPLAY_DC_PIN, DISPLAY_RST_PIN_1_44_INCH_LCD_ILI9163, 4, 0);
    init_and_clear(ili9163, QP_ROTATION_90);

    st7789 = qp_st7789_make_spi_device(240, 240, DISPLAY_CS_PIN_1_3_INCH_LCD_ST7789, DISPLAY_DC_PIN, DISPLAY_RST_PIN_1_3_INCH_LCD_ST7789, 2, 3);
    init_and_clear(st7789, QP_ROTATION_270);

    ssd1351 = qp_ssd1351_make_spi_device(128, 128, DISPLAY_CS_PIN_1_5_INCH_OLED_SSD1351, DISPLAY_DC_PIN, DISPLAY_RST_PIN_1_5_INCH_OLED_SSD1351, 8, 0);
    init_and_clear(ssd1351, QP_ROTATION_180);
}

void matrix_scan_kb(void) {
    static uint32_t last_scan = 0;
    uint32_t        now       = timer_read32();
    if (TIMER_DIFF_32(now, last_scan) >= 8000) {
        last_scan = now;
        draw_test(ili9163, "ILI9163", now);
        draw_test(st7789, "ST7789", now);
        draw_test(ssd1351, "SSD1351", now);

        static bool animating = false;
        if (!animating) {
            animating = true;
            if (test_anim) {
                qp_animate(ili9163, 0, 0, test_anim);
                qp_animate(st7789, 0, 0, test_anim);
                qp_animate(ssd1351, 0, 0, test_anim);
            }

            if (loading) {
                qp_animate(ili9163, 0, 8, loading);
                qp_animate(st7789, 0, 8, loading);
                qp_animate(ssd1351, 0, 8, loading);
            }
        }
    }
}
