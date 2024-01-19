// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

void housekeeping_task_kb(void) {
    // Go into low-scan interrupt-based mode if we haven't had any matrix activity in the last 5 seconds
    if (last_input_activity_elapsed() > 5000) {
        // ROW2COL
        const pin_t row_pins[] = MATRIX_ROW_PINS;
        const pin_t col_pins[] = MATRIX_COL_PINS;

        // Set up row/col pins and attach callback
        for (int i = 0; i < sizeof(col_pins) / sizeof(pin_t); ++i) {
            setPinOutput(col_pins[i]);
            writePinLow(col_pins[i]);
        }
        for (int i = 0; i < sizeof(row_pins) / sizeof(pin_t); ++i) {
            setPinInputHigh(row_pins[i]);
            palEnableLineEvent(row_pins[i], PAL_EVENT_MODE_BOTH_EDGES);
        }

        // Wait for an interrupt
        __WFI();

        // Now that the interrupt has woken us up, reset all the row/col pins back to defaults
        for (int i = 0; i < sizeof(row_pins) / sizeof(pin_t); ++i) {
            palDisableLineEvent(row_pins[i]);
            writePinHigh(row_pins[i]);
            setPinInputHigh(row_pins[i]);
        }
        for (int i = 0; i < sizeof(col_pins) / sizeof(pin_t); ++i) {
            writePinHigh(col_pins[i]);
            setPinInputHigh(col_pins[i]);
        }
    }
}
