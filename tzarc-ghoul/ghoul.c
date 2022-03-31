// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "ghoul.h"
#include "analog.h"
#include "spi_master.h"

void ui_init(void);
void ui_task(void);

void early_hardware_init_post(void) {
    // Forcefully disable the RGB output
    setPinOutput(RGB_ENABLE_PIN);
    writePinLow(RGB_ENABLE_PIN);
}

void keyboard_post_init_kb(void) {
    debug_enable   = true;
    debug_matrix   = true;
    debug_keyboard = true;

    // Enable RGB current limiter and wait for a bit before allowing RGB to continue
    writePinHigh(RGB_ENABLE_PIN);
    wait_ms(20);

    // Init the display
    ui_init();
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

void housekeeping_task_kb(void) {
    ui_task();
}