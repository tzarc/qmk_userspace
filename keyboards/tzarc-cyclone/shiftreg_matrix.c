// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#include "quantum.h"

#if (!defined(SHIFTREG_MATRIX_ROW_LATCH) || !defined(SHIFTREG_MATRIX_ROW_CLK) || !defined(SHIFTREG_MATRIX_ROW_DATA) || !defined(SHIFTREG_MATRIX_COL_LATCH) || !defined(SHIFTREG_MATRIX_COL_CLK) || !defined(SHIFTREG_MATRIX_COL_DATA))
#    error Missing shift register I/O pin definitions
#endif

void pulsePinHigh(pin_t pin) {
    writePinHigh(pin);
    matrix_io_delay();
    writePinLow(pin);
}

void pulsePinLow(pin_t pin) {
    writePinLow(pin);
    matrix_io_delay();
    writePinHigh(pin);
}

void matrix_init_custom(void) {
    // Set up the initial states for all the output pins
    writePinLow(SHIFTREG_MATRIX_ROW_LATCH);
    setPinOutput(SHIFTREG_MATRIX_ROW_LATCH);

    writePinLow(SHIFTREG_MATRIX_ROW_CLK);
    setPinOutput(SHIFTREG_MATRIX_ROW_CLK);

    writePinLow(SHIFTREG_MATRIX_ROW_DATA);
    setPinOutput(SHIFTREG_MATRIX_ROW_DATA);

    writePinHigh(SHIFTREG_MATRIX_COL_LATCH);
    setPinOutput(SHIFTREG_MATRIX_COL_LATCH);

    writePinLow(SHIFTREG_MATRIX_COL_CLK);
    setPinOutput(SHIFTREG_MATRIX_COL_CLK);

    // Set up the column input pin
#if defined(__AVR__)
    setPinInput(SHIFTREG_MATRIX_COL_DATA);
#else
    setPinInputLow(SHIFTREG_MATRIX_COL_DATA);
#endif

    // Clear the row driver
    for (uint8_t i = 0; i < MATRIX_ROWS; ++i) {
        matrix_io_delay();
        pulsePinHigh(SHIFTREG_MATRIX_ROW_CLK);
    }

    // Latch the row driver
    matrix_io_delay();
    pulsePinHigh(SHIFTREG_MATRIX_ROW_LATCH);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    // Keep track of if something was modified
    bool matrix_has_changed = false;

    // Write zeros to the row driver
    writePinLow(SHIFTREG_MATRIX_ROW_DATA);
    for (uint8_t r = 0; r < MATRIX_ROWS - 1; ++r) {
        matrix_io_delay();
        pulsePinHigh(SHIFTREG_MATRIX_ROW_CLK);
    }

    // Clock a single '1' on the row driver to enable the first row
    matrix_io_delay();
    writePinHigh(SHIFTREG_MATRIX_ROW_DATA);
    matrix_io_delay();
    pulsePinHigh(SHIFTREG_MATRIX_ROW_CLK);
    writePinLow(SHIFTREG_MATRIX_ROW_DATA);

    // Latch the row driver
    matrix_io_delay();
    pulsePinHigh(SHIFTREG_MATRIX_ROW_LATCH);

    for (int r = 0; r < MATRIX_ROWS; ++r) {
        matrix_row_t row_rx = 0;

        // Latch the column input register -- a 74HC165 puts D7 onto Q7, no need to clock initially
        matrix_io_delay();
        pulsePinLow(SHIFTREG_MATRIX_COL_LATCH);

        // Read each of the column values
        for (int c = 0; c < MATRIX_COLS; ++c) {
            matrix_io_delay();
            if (readPin(SHIFTREG_MATRIX_COL_DATA)) {
                row_rx |= ((matrix_row_t)1) << c;
            }

            // Clock the column input register
            pulsePinHigh(SHIFTREG_MATRIX_COL_CLK);
        }

        // Shift the row driver up by 1, proceeding to the next row
        matrix_io_delay();
        pulsePinHigh(SHIFTREG_MATRIX_ROW_CLK);
        matrix_io_delay();
        pulsePinHigh(SHIFTREG_MATRIX_ROW_LATCH);

        // Check for modifications and copy to the matrix
        if (current_matrix[r] != row_rx) {
            matrix_has_changed = true;
            current_matrix[r]  = row_rx;
        }
    }

    return matrix_has_changed;
}
