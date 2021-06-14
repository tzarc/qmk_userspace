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

    setPinOutput(SPI_MATRIX_LATCH_PIN);
    writePinLow(SPI_MATRIX_LATCH_PIN);
    setPinOutput(SPI_MATRIX_PLOAD_PIN);
    writePinLow(SPI_MATRIX_PLOAD_PIN);
    setPinOutput(SPI_MATRIX_CHIP_SELECT_PIN);
    writePinHigh(SPI_MATRIX_CHIP_SELECT_PIN);
}

void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    static bool spi_initialised = false;
    if (!spi_initialised) {
        spi_init();
        spi_initialised = true;
    }

    if (current_row == 0) {
        // Pulse the latch pin
        writePinHigh(SPI_MATRIX_PLOAD_PIN);
        writePinHigh(SPI_MATRIX_LATCH_PIN);
        matrix_io_delay();
        writePinLow(SPI_MATRIX_LATCH_PIN);

        // Read from SPI the matrix
        spi_start(SPI_MATRIX_CHIP_SELECT_PIN, false, 0, 4);
        spi_receive(current_matrix, SPI_MATRIX_NUMBER_REGISTERS);
        spi_stop();

        writePinLow(SPI_MATRIX_PLOAD_PIN);

        static matrix_row_t last_matrix[MATRIX_ROWS] = {0xFF};
        if (memcmp(last_matrix, current_matrix, sizeof(last_matrix)) != 0) {
            memcpy(last_matrix, current_matrix, sizeof(last_matrix));
            matrix_print();
        }
    }
}