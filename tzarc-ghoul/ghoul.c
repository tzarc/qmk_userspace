// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "ghoul.h"
#include "analog.h"
#include "spi_master.h"
#include "qp.h"
#include "qp_ssd1351.h"

#include "ghoul-logo.qgf.c"
#include "thintel15.qff.c"

static painter_device_t       oled;
static painter_image_handle_t logo;
static painter_font_handle_t  font;

void early_hardware_init_post(void) {
    // Forcefully disable the RGB output
    setPinOutput(RGB_ENABLE_PIN);
    writePinLow(RGB_ENABLE_PIN);
}

void keyboard_post_init_kb(void) {
    debug_enable   = true;
    debug_matrix   = true;
    debug_keyboard = true;

    oled = qp_ssd1351_make_spi_device(128, 128, OLED_CS_PIN, OLED_DC_PIN, OLED_RST_PIN, 8, 0);
    logo = qp_load_image_mem(gfx_ghoul_logo);
    font = qp_load_font_mem(font_thintel15);

    qp_init(oled, QP_ROTATION_90);
    qp_rect(oled, 0, 0, 127, 127, 0, 0, 0, true);
    qp_drawimage_recolor(oled, 127 - logo->width, 0, logo, 0, 255, 255, 0, 255, 0);
    qp_drawtext(oled, 0, 0, font, "Ghoul v1.0");
    qp_flush(oled);

    // Enable RGB current limiter and wait for a bit before allowing RGB to continue
    writePinHigh(RGB_ENABLE_PIN);
    wait_ms(50);
}

void matrix_init_custom(void) {
    // SPI Matrix
    setPinOutput(SPI_MATRIX_CHIP_SELECT_PIN);
    writePinHigh(SPI_MATRIX_CHIP_SELECT_PIN);
    spi_init();

    // Encoder pushbutton
    setPinInputLow(ENCODER_PUSHBUTTON_PIN);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    static matrix_row_t temp_matrix[MATRIX_ROWS] = {0};

    // Read from SPI the matrix
    spi_start(SPI_MATRIX_CHIP_SELECT_PIN, false, 0, 16);
    spi_receive((uint8_t*)temp_matrix, MATRIX_SHIFT_REGISTER_COUNT * sizeof(matrix_row_t));
    spi_stop();

    // Read from the encoder pushbutton
    temp_matrix[5] = readPin(ENCODER_PUSHBUTTON_PIN) ? 1 : 0;

    // Check if we've changed, return the last-read data
    bool changed = memcmp(current_matrix, temp_matrix, sizeof(temp_matrix)) != 0;
    memcpy(current_matrix, temp_matrix, sizeof(temp_matrix));
    return changed;
}

#define NUM_ADC_READS 32

void housekeeping_task_kb(void) {
    bool            hue_redraw = false;
    static uint16_t last_hue   = 0xFFFF;
    uint8_t         curr_hue   = rgblight_get_hue();
    if (last_hue != curr_hue) {
        last_hue   = curr_hue;
        hue_redraw = true;
    }

    if (hue_redraw) {
        qp_drawimage_recolor(oled, 127 - logo->width, 0, logo, curr_hue, 255, 255, curr_hue, 255, 0);
    }

    static int16_t current_reads[NUM_ADC_READS] = {0};
    static int16_t voltage_reads[NUM_ADC_READS] = {0};
    static int     write_offset                 = 0;

    static uint32_t last_read = 0;
    if (timer_elapsed32(last_read) >= 1) {
        // Perform the reads
        int16_t current    = analogReadPin(ADC_CURRENT_PIN);
        int16_t voltage    = analogReadPin(ADC_VOLTAGE_PIN);
        int16_t current_ma = (int16_t)(((3300 * (int32_t)current) / ADC_SATURATION));
        int16_t voltage_mv = (int16_t)(2 * (3300 * (int32_t)voltage) / ADC_SATURATION);

        // Duplicate the first read so that averages work
        if (last_read == 0) {
            for (int i = 0; i < NUM_ADC_READS; ++i) {
                current_reads[i] = current_ma;
                voltage_reads[i] = voltage_mv;
            }
        }

        // Dump in the current value
        current_reads[write_offset] = current_ma;
        voltage_reads[write_offset] = voltage_mv;
        write_offset                = (write_offset + 1) % NUM_ADC_READS;

        last_read = timer_read32();
    }

    static uint32_t last_draw = 0;
    if (hue_redraw || timer_elapsed32(last_draw) >= 250) {
        // Accumulate
        int32_t total_current_ma = 0;
        int32_t total_voltage_mv = 0;
        for (int i = 0; i < NUM_ADC_READS; ++i) {
            total_current_ma += current_reads[i];
            total_voltage_mv += voltage_reads[i];
        }

        // Get the averages
        int16_t avg_current_ma = (int16_t)(total_current_ma / NUM_ADC_READS);
        int16_t avg_voltage_mv = (int16_t)(total_voltage_mv / NUM_ADC_READS);

        char buf[32] = {0};
        sprintf(buf, "Current: %dmA", avg_current_ma);
        qp_drawtext(oled, 0, 127 - (font->line_height * 2), font, buf);
        sprintf(buf, "Voltage: %dmV", avg_voltage_mv);
        qp_drawtext(oled, 0, 127 - (font->line_height * 1), font, buf);
        qp_flush(oled);

        last_draw = timer_read32();
    }
}