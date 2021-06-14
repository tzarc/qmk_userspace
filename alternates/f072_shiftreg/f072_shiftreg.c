/* Copyright 2018-2021 Nick Brassel (@tzarc)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <quantum.h>
#include <string.h>
#include <spi_master.h>
#include <timer.h>

void matrix_io_delay(void) { __asm__ volatile("nop\nnop\nnop\n"); }

void keyboard_post_init_kb(void) {
    debug_enable   = true;
    debug_matrix   = true;
    debug_keyboard = true;
}

void matrix_init_pins(void) {
    setPinOutput(SPI_MATRIX_LATCH_PIN);
    writePinLow(SPI_MATRIX_LATCH_PIN);
    setPinOutput(SPI_MATRIX_CHIP_SELECT_PIN);
    writePinHigh(SPI_MATRIX_CHIP_SELECT_PIN);
    spi_init();
}

void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    if (current_row == 0) {
        writePinHigh(SPI_MATRIX_LATCH_PIN);

        // Read from SPI the matrix
        spi_start(SPI_MATRIX_CHIP_SELECT_PIN, false, 0, 4);
        spi_receive(current_matrix, SPI_MATRIX_NUMBER_REGISTERS);
        spi_stop();

        writePinLow(SPI_MATRIX_LATCH_PIN);
    }
}