// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later

#include <quantum.h>
#include <debug.h>
#include <qp.h>
#include <qp_lvgl.h>

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

painter_device_t lcd;

void lv_example_arc_2(void);

void keyboard_post_init_kb(void) {
    debug_enable   = true;
    debug_matrix   = true;
    debug_keyboard = true;

    lcd = qp_st7789_make_spi_device(240, 320, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, 4, 3);
    qp_init(lcd, QP_ROTATION_0);
    qp_rect(lcd, 0, 0, 239, 319, 0, 255, 255, true);
    return;

    if (qp_lvgl_attach(lcd)) {
        lv_example_arc_2();
    }

    keyboard_post_init_user();
}

static void set_angle(void* obj, int32_t v) {
    lv_arc_set_value(obj, v);
    dprintf("arc:%d\n", (int)v);
}

/**
 * Create an arc which acts as a loader.
 */
void lv_example_arc_2(void) {
    /*Create an Arc*/
    lv_obj_t* arc = lv_arc_create(lv_scr_act());
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);  /*Be sure the knob is not displayed*/
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE); /*To not allow adjusting by click*/
    lv_obj_center(arc);

    static lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, arc);
    lv_anim_set_exec_cb(&a, set_angle);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE); /*Just for the demo*/
    lv_anim_set_repeat_delay(&a, 500);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_start(&a);
}