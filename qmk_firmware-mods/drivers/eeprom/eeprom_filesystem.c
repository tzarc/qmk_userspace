// Copyright 2022-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdint.h>
#include <string.h>

#include "quantum.h"
#include "eeprom_driver.h"
#include "filesystem.h"

#ifndef EEPROM_FILESYSTEM_CHUNK_SIZE
#    define EEPROM_FILESYSTEM_CHUNK_SIZE 16
#endif // EEPROM_FILESYSTEM_CHUNK_SIZE

void eeprom_driver_init(void) {
    fs_init();
    fs_mkdir("ee");
}

void eeprom_driver_erase(void) {
    fs_rmdir("ee", true);
}

void eeprom_read_block(void *buf, const void *addr, size_t len) {
    uintptr_t offset = (uintptr_t)addr;
    uint8_t *ptr = (uint8_t *)buf;

    int first_block = offset / EEPROM_FILESYSTEM_CHUNK_SIZE;
    int last_block = (offset + len + EEPROM_FILESYSTEM_CHUNK_SIZE - 1) / EEPROM_FILESYSTEM_CHUNK_SIZE;
    for (int i = first_block; i < last_block; i++) {
        char path[8] = {0};
        strlcpy(path, "ee/", sizeof(path));
        get_numeric_str(path + 3, sizeof(path) - 3, i, '0');
        fs_fd_t fd = fs_open(path, "w");
        if (fd < 0) {
            return;
        }

        fs_size_t this_offset = offset % EEPROM_FILESYSTEM_CHUNK_SIZE;
        fs_seek(fd, this_offset, FS_SEEK_SET);
        fs_size_t this_len = MIN(len, EEPROM_FILESYSTEM_CHUNK_SIZE - (offset % EEPROM_FILESYSTEM_CHUNK_SIZE));
        fs_size_t read_len = fs_read(fd, ptr, this_len);
        fs_close(fd);
        if(read_len != this_len) {
            return;
        }
        ptr += this_len;
        offset += this_len;
    }
}

void eeprom_write_block(const void *buf, void *addr, size_t len) {
    uintptr_t offset = (uintptr_t)addr;
    const uint8_t *ptr = (const uint8_t *)buf;

    int first_block = offset / EEPROM_FILESYSTEM_CHUNK_SIZE;
    int last_block = (offset + len + EEPROM_FILESYSTEM_CHUNK_SIZE - 1) / EEPROM_FILESYSTEM_CHUNK_SIZE;
    for (int i = first_block; i < last_block; i++) {
        char path[8] = {0};
        strlcpy(path, "ee/", sizeof(path));
        get_numeric_str(path + 3, sizeof(path) - 3, i, '0');
        fs_fd_t fd = fs_open(path, "w");
        if (fd < 0) {
            return;
        }

        fs_size_t this_offset = offset % EEPROM_FILESYSTEM_CHUNK_SIZE;
        fs_seek(fd, this_offset, FS_SEEK_SET);
        fs_size_t this_len = MIN(len, EEPROM_FILESYSTEM_CHUNK_SIZE - (offset % EEPROM_FILESYSTEM_CHUNK_SIZE));
        fs_size_t write_len = fs_write(fd, ptr, this_len);
        fs_close(fd);
        if(write_len != this_len) {
            return;
        }
        ptr += this_len;
        offset += this_len;
    }
}
