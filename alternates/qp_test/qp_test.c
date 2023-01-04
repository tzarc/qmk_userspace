// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <quantum.h>
#include <debug.h>
#include <qp.h>
#include <qp_st7789.h>

// Test assets
#include "graphics/test-image.qgf.c"
#include "graphics/test-anim.qgf.c"
#include "graphics/loading.qgf.c"
#include "graphics/thintel.qff.c"

/*
void board_init(void) {
    // Add a long delay during bootup to allow for debugger attachment
    int f;
    for (int i = 0; i < 10000; ++i) {
        for (int j = 0; j < 10000; ++j) {
            __asm__ volatile("nop\nnop\nnop\n");
        }
        ++f;
    }
}
*/

painter_device_t       st7789;
painter_image_handle_t test_image;
painter_image_handle_t test_anim;
painter_image_handle_t loading;

painter_font_handle_t thintel;

static uint32_t delayed_test(uint32_t trigger_time, void* cb_arg) {
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

void draw_test(painter_device_t device, const char* name, uint32_t now) {
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

    defer_exec(3000, delayed_test, (void*)(uint16_t)3000);
    defer_exec(2900, delayed_test, (void*)(uint16_t)2900);

    test_image = qp_load_image_mem(gfx_test_image);
    test_anim  = qp_load_image_mem(gfx_test_anim);
    loading    = qp_load_image_mem(gfx_loading);

    thintel = qp_load_font_mem(font_thintel);

    st7789 = qp_st7789_make_spi_device(240, 320, DISPLAY_CS_PIN_2_0_INCH_LCD_ST7789, DISPLAY_DC_PIN, DISPLAY_RST_PIN_2_0_INCH_LCD_ST7789, 2, 3);
    init_and_clear(st7789, QP_ROTATION_0);

    keyboard_post_init_user();
}

void matrix_scan_kb(void) {
    static uint32_t last_scan = 0;
    uint32_t        now       = timer_read32();
    if (TIMER_DIFF_32(now, last_scan) >= 3000) {
        last_scan = now;
        draw_test(st7789, "ST7789", now);

        static bool animating = false;
        if (!animating) {
            animating = true;
            if (test_anim) {
                qp_animate(st7789, 0, 0, test_anim);
            }

            if (loading) {
                qp_animate(st7789, 0, 8, loading);
            }
        }
    }

    matrix_scan_user();
}

void chibi_system_halt_hook(const char* reason) {
    // re-route to QMK toolbox...
    uprintf("system halting: %s\n", reason);
}

void chibi_system_trace_hook(void* tep) {
    // re-route to QMK toolbox...
    uprintf("trace\n");
}

void chibi_debug_check_hook(const char* func, const char* condition, int value) {
    // re-route to QMK toolbox...
    uprintf("%s debug check failure: (%s) == %s\n", func, condition, value ? "true" : "false");
    // ...and hard-loop fail
    while (1) {
    }
}

void chibi_debug_assert_hook(const char* func, const char* condition, int value, const char* reason) {
    // re-route to QMK toolbox...
    uprintf("%s debug assert (%s) failure: (%s) == %s\n", func, reason, condition, value ? "true" : "false");
    // ...and hard-loop fail
    while (1) {
    }
}

#ifdef DEBUG_EEPROM_OUTPUT

#    ifdef WEAR_LEVELING_ENABLE
#        include "wear_leveling.h"
#    endif // WEAR_LEVELING_ENABLE

uint8_t prng(void) {
    static uint8_t s = 0xAA, a = 0;
    s ^= s << 3;
    s ^= s >> 5;
    s ^= a++ >> 2;
    return s;
}

void keyboard_post_init_user(void) {
    debug_enable = true;
}

void matrix_scan_user(void) {
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
        dprint("init'ing wear-leveling\n");
        last_wear_leveling_init = now;
        wear_leveling_init();
    }
#    endif // WEAR_LEVELING_ENABLE
}

#endif // DEBUG_EEPROM_OUTPUT
