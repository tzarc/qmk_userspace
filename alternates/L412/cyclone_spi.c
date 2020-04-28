/* Copyright 2018-2020 Nick Brassel (@tzarc)
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

#include "ch.h"
#include <hal.h>
#include "cyclone.h"

#define SPI1_SCK 5
#define SPI1_MISO 6
#define SPI1_MOSI 7

#define SPI1_SCK_ALTMODE 5
#define SPI1_MISO_ALTMODE 5
#define SPI1_MOSI_ALTMODE 5

void cyclone_spi_init(void) {
    // Try releasing special pins for a short time
    palSetPadMode(GPIOA, SPI1_SCK, PAL_MODE_INPUT);
    palSetPadMode(GPIOA, SPI1_MISO, PAL_MODE_INPUT);
    palSetPadMode(GPIOA, SPI1_MOSI, PAL_MODE_INPUT);

    chThdSleepMilliseconds(10);

    palSetPadMode(GPIOA, SPI1_SCK, PAL_MODE_ALTERNATE(SPI1_SCK_ALTMODE) | PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, SPI1_MISO, PAL_MODE_ALTERNATE(SPI1_MISO_ALTMODE) | PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, SPI1_MOSI, PAL_MODE_ALTERNATE(SPI1_MOSI_ALTMODE) | PAL_MODE_OUTPUT_PUSHPULL);
}
