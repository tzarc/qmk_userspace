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
