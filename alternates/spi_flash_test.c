// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later

void housekeeping_task_kb(void) {
#ifdef EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN
    static bool spiflash_tested = false;
    if(!spiflash_tested && timer_read32() > 15000) {
        spiflash_tested = true;
        flash_init();

        if(spi_start(EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN, EXTERNAL_FLASH_SPI_LSBFIRST, EXTERNAL_FLASH_SPI_MODE, EXTERNAL_FLASH_SPI_CLOCK_DIVISOR)) {
            spi_write(0x9F);
            uint8_t jedec_id[3];
            spi_receive(jedec_id, sizeof(jedec_id));
            spi_stop();
            dprintf("JEDEC ID: %02X %02X %02X\n", (int)jedec_id[0], (int)jedec_id[1], (int)jedec_id[2]);
        }

        if(flash_erase_chip() == FLASH_STATUS_SUCCESS) {
            const uint8_t test_data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
            uint8_t recv_data[sizeof(test_data)] = {0};
            dprint("Reading initial state:\n");
            flash_read_block(0, recv_data, sizeof(recv_data));
            dprint("Writing test data:\n");
            flash_write_block(0, test_data, sizeof(test_data));
            dprint("Reading test data:\n");
            flash_read_block(0, recv_data, sizeof(recv_data));
        }
    }
#endif // EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN
}
