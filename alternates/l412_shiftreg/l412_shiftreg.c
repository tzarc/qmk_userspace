// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <quantum.h>
#include <spi_master.h>

void keyboard_post_init_kb(void) {
    debug_enable   = true;
    debug_matrix   = true;
    debug_keyboard = true;
}

void matrix_init_pins(void) {
#ifdef SPI_MATRIX_LATCH_PIN
    setPinOutput(SPI_MATRIX_LATCH_PIN);
    writePinLow(SPI_MATRIX_LATCH_PIN);
#endif // SPI_MATRIX_LATCH_PIN

#ifdef SPI_MATRIX_PLOAD_PIN
    setPinOutput(SPI_MATRIX_PLOAD_PIN);
    writePinLow(SPI_MATRIX_PLOAD_PIN);
#endif // SPI_MATRIX_PLOAD_PIN

    setPinOutput(SPI_MATRIX_CHIP_SELECT_PIN);
    writePinHigh(SPI_MATRIX_CHIP_SELECT_PIN);
    spi_init();
}

void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    if (current_row == 0) {
#ifdef SPI_MATRIX_LATCH_PIN
        writePinHigh(SPI_MATRIX_LATCH_PIN);
#endif // SPI_MATRIX_LATCH_PIN

#ifdef SPI_MATRIX_PLOAD_PIN
        writePinHigh(SPI_MATRIX_PLOAD_PIN);
#endif // SPI_MATRIX_PLOAD_PIN

        // Read from SPI the matrix
        spi_start(SPI_MATRIX_CHIP_SELECT_PIN, false, 0, 4);
        spi_receive(current_matrix, MATRIX_ROWS);
        spi_stop();

#ifdef SPI_MATRIX_PLOAD_PIN
        writePinLow(SPI_MATRIX_PLOAD_PIN);
#endif // SPI_MATRIX_PLOAD_PIN

#ifdef SPI_MATRIX_LATCH_PIN
        writePinLow(SPI_MATRIX_LATCH_PIN);
#endif // SPI_MATRIX_LATCH_PIN
    }
}
