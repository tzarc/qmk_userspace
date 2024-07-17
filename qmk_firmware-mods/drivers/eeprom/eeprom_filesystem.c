// Copyright 2022-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdint.h>
#include <string.h>

#include "flash_spi.h"
#include "quantum.h"
#include "eeprom_driver.h"
#include "filesystem.h"
#include "util.h"

// #define DEBUG_EEPROM_OUTPUT

#if defined(CONSOLE_ENABLE) && defined(DEBUG_EEPROM_OUTPUT)
#    include "timer.h"
#    include "debug.h"
#endif // DEBUG_EEPROM_OUTPUT

#ifndef EEPROM_FILESYSTEM_CHUNK_SIZE
#    define EEPROM_FILESYSTEM_CHUNK_SIZE 16
#endif // EEPROM_FILESYSTEM_CHUNK_SIZE

static uint8_t eeprom_cache[EEPROM_SIZE];
static uint8_t eeprom_dirty[(EEPROM_SIZE) / (EEPROM_FILESYSTEM_CHUNK_SIZE) / 8];
_Static_assert((((((EEPROM_SIZE) / (EEPROM_FILESYSTEM_CHUNK_SIZE)) / 8) * 8) * (EEPROM_FILESYSTEM_CHUNK_SIZE)) == (EEPROM_SIZE), "EEPROM_SIZE must be a multiple of (EEPROM_FILESYSTEM_CHUNK_SIZE * 8)");

static void ee_filename(int index, char *buf, size_t len) {
    strlcpy(buf, "ee/", len);
    get_numeric_str(buf + 3, len - 3, index, '0');
}

void eeprom_driver_init(void) {
    memset(eeprom_dirty, 0, sizeof(eeprom_dirty));
    memset(eeprom_cache, 0, sizeof(eeprom_cache));
    fs_init();
    fs_mkdir("ee");
    for (int i = 0; i < ARRAY_SIZE(eeprom_cache); i += (EEPROM_FILESYSTEM_CHUNK_SIZE)) {
        int  chunk_index = i / (EEPROM_FILESYSTEM_CHUNK_SIZE);
        char path[8]     = {0};
        ee_filename(chunk_index, path, sizeof(path));
        if (!fs_exists(path)) {
            continue;
        }
        fs_fd_t fd = fs_open(path, FS_READ);
        if (fd == INVALID_FILESYSTEM_FD) {
            continue;
        }
        fs_size_t read = fs_read(fd, &eeprom_cache[i], (EEPROM_FILESYSTEM_CHUNK_SIZE));
        if (read != (EEPROM_FILESYSTEM_CHUNK_SIZE)) {
            memset(&eeprom_cache[i], 0, (EEPROM_FILESYSTEM_CHUNK_SIZE));
        }
        fs_close(fd);
    }
}

void eeprom_driver_erase(void) {
    while(fs_is_mounted()) {
        fs_unmount();
    }
    flash_erase_chip();
    eeprom_driver_init();
}

void eeprom_driver_flush(void) {
    for (int i = 0; i < ARRAY_SIZE(eeprom_dirty); ++i) {
        if (eeprom_dirty[i]) {
            for (int j = 0; j < 8; ++j) {
                if (eeprom_dirty[i] & (1 << j)) {
                    int      chunk_index = i * 8 + j;
                    uint8_t *ptr         = &eeprom_cache[chunk_index * (EEPROM_FILESYSTEM_CHUNK_SIZE)];
                    char     path[8]     = {0};
                    ee_filename(chunk_index, path, sizeof(path));
                    fs_fd_t fd = fs_open(path, FS_WRITE | FS_TRUNCATE);
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
    memcpy(buf, &eeprom_cache[offset], len);

#if defined(CONSOLE_ENABLE) && defined(DEBUG_EEPROM_OUTPUT)
    dprintf("[EEPROM R] 0x%04X: ", ((int)addr));
    for (size_t i = 0; i < len; ++i) {
        dprintf(" %02X", (int)(((uint8_t *)buf)[i]));
    }
    dprintf("\n");
#endif // DEBUG_EEPROM_OUTPUT
}

void eeprom_write_block(const void *buf, void *addr, size_t len) {
    uintptr_t      offset = (uintptr_t)addr;
    const uint8_t *ptr    = (const uint8_t *)buf;

#if defined(CONSOLE_ENABLE) && defined(DEBUG_EEPROM_OUTPUT)
        dprintf("[EEPROM W] 0x%04X: ", ((int)addr));
        for (uint8_t i = 0; i < len; i++) {
            dprintf(" %02X", (int)(((uint8_t*)buf)[i]));
        }
        dprintf("\n");
#endif // DEBUG_EEPROM_OUTPUT

    // Ugly memcpy() with dirty flag updates.
    for (size_t i = 0; i < len; ++i) {
        size_t target_offset = offset + i;
        if (eeprom_cache[target_offset] != ptr[i]) {
            eeprom_cache[target_offset] = ptr[i];
            size_t chunk_index            = target_offset / (EEPROM_FILESYSTEM_CHUNK_SIZE);
            eeprom_dirty[chunk_index / 8] |= 1 << (chunk_index % 8);
        }
    }
}
