// Copyright 2022-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdint.h>
#include <string.h>

#include "quantum.h"
#include "eeprom_driver.h"
#include "filesystem.h"
#include "util.h"

#ifndef EEPROM_FILESYSTEM_CHUNK_SIZE
#    define EEPROM_FILESYSTEM_CHUNK_SIZE 16
#endif // EEPROM_FILESYSTEM_CHUNK_SIZE

static uint8_t eeprom_backing[EEPROM_SIZE];
static uint8_t eeprom_dirty[(EEPROM_SIZE) / (EEPROM_FILESYSTEM_CHUNK_SIZE) / 8];
_Static_assert((((((EEPROM_SIZE) / (EEPROM_FILESYSTEM_CHUNK_SIZE)) / 8) * 8) * (EEPROM_FILESYSTEM_CHUNK_SIZE)) == (EEPROM_SIZE), "EEPROM_SIZE must be a multiple of (EEPROM_FILESYSTEM_CHUNK_SIZE * 8)");

static void ee_filename(int index, char *buf, size_t len) {
    strlcpy(buf, "ee/", len);
    get_numeric_str(buf + 3, len - 3, index, '0');
}

void eeprom_driver_init(void) {
    memset(eeprom_dirty, 0, sizeof(eeprom_dirty));
    memset(eeprom_backing, 0, sizeof(eeprom_backing));
    fs_init();
    fs_mkdir("ee");
    for (int i = 0; i < ARRAY_SIZE(eeprom_backing); i += (EEPROM_FILESYSTEM_CHUNK_SIZE)) {
        int  chunk_index = i / (EEPROM_FILESYSTEM_CHUNK_SIZE);
        char path[8]     = {0};
        ee_filename(chunk_index, path, sizeof(path));
        if (!fs_exists(path)) {
            continue;
        }
        fs_fd_t fd = fs_open(path, "r");
        if (fd == INVALID_FILESYSTEM_FD) {
            continue;
        }
        fs_size_t read = fs_read(fd, &eeprom_backing[i], (EEPROM_FILESYSTEM_CHUNK_SIZE));
        if (read != (EEPROM_FILESYSTEM_CHUNK_SIZE)) {
            memset(&eeprom_backing[i], 0, (EEPROM_FILESYSTEM_CHUNK_SIZE));
        }
        fs_close(fd);
    }
}

void eeprom_driver_erase(void) {
    memset(eeprom_dirty, 0, sizeof(eeprom_dirty));
    memset(eeprom_backing, 0, sizeof(eeprom_backing));
    fs_rmdir("ee", true);
}

void eeprom_driver_flush(void) {
    for (int i = 0; i < ARRAY_SIZE(eeprom_dirty); ++i) {
        if (eeprom_dirty[i]) {
            for (int j = 0; j < 8; ++j) {
                if (eeprom_dirty[i] & (1 << j)) {
                    int      chunk_index = i * 8 + j;
                    uint8_t *ptr         = &eeprom_backing[chunk_index * (EEPROM_FILESYSTEM_CHUNK_SIZE)];
                    char     path[8]     = {0};
                    ee_filename(chunk_index, path, sizeof(path));
                    fs_fd_t fd = fs_open(path, "wt");
                    if (fd == INVALID_FILESYSTEM_FD) {
                        continue;
                    }
                    fs_write(fd, ptr, (EEPROM_FILESYSTEM_CHUNK_SIZE));
                    fs_close(fd);
                }
            }
            eeprom_dirty[i] = 0;
        }
    }
}

void eeprom_read_block(void *buf, const void *addr, size_t len) {
    uintptr_t offset = (uintptr_t)addr;
    memcpy(buf, &eeprom_backing[offset], len);
}

void eeprom_write_block(const void *buf, void *addr, size_t len) {
    uintptr_t      offset = (uintptr_t)addr;
    const uint8_t *ptr    = (const uint8_t *)buf;

    // Ugly memcpy() with dirty flag updates.
    for (size_t i = 0; i < len; ++i) {
        size_t target_offset = offset + i;
        if (eeprom_backing[target_offset] != ptr[i]) {
            eeprom_backing[target_offset] = ptr[i];
            size_t chunk_index            = target_offset / (EEPROM_FILESYSTEM_CHUNK_SIZE);
            eeprom_dirty[chunk_index / 8] |= 1 << (chunk_index % 8);
        }
    }
}
