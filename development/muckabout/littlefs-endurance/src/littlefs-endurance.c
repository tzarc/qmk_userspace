// Copyright 2022-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdio.h>

#include "lfs.h"

#define SPACING 128
#define BUFFER_SIZE 128
#define LOOKAHEAD_SIZE 128

// Emulation on 8kB EEPROM
#define BLOCK_COUNT 64
#define BLOCK_SIZE 128
const int erase_limit = 100000;

#define BYTES_PER_SLICE 32
#define TOTAL_EEPROM_SIZE 512
#define TOTAL_EEPROM_SLICES ((TOTAL_EEPROM_SIZE) / (BYTES_PER_SLICE))

#define EEPROM_DIR_NAME "ee"
#define EEPROM_DIR_SEPARATOR "/"
// #define EEPROM_DIR_NAME ""
// #define EEPROM_DIR_SEPARATOR ""
#define EEPROM_FILE_PREFIX "e"

static uint64_t splitmix64_state; /* The state can be seeded with any value. */

#define LFS_API_CALL(api, ...)                  \
    ({                                          \
        int ret = api(__VA_ARGS__);             \
        if (ret < 0) {                          \
            printf(#api " returned %d\n", ret); \
        }                                       \
        ret;                                    \
    })

uint64_t splitmix64_next() {
    uint64_t z = (splitmix64_state += 0x9e3779b97f4a7c15);
    z          = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z          = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}

static struct {
    uint8_t    barrier0[SPACING];
    uint8_t    ramdisk[BLOCK_COUNT][BLOCK_SIZE];
    uint8_t    barrier1[SPACING];
    uint8_t    lookahead_buf[LOOKAHEAD_SIZE];
    uint8_t    barrier2[SPACING];
    uint8_t    read_buf[BUFFER_SIZE];
    uint8_t    barrier3[SPACING];
    uint8_t    prog_buf[BUFFER_SIZE];
    uint8_t    barrier4[SPACING];
    lfs_t      lfs;
    uint8_t    barrier5[SPACING + 64 - (sizeof(lfs_t) % 64)];
    lfs_file_t file;
    uint8_t    barrier6[SPACING + 64 - (sizeof(lfs_file_t) % 64)];
    int        erase_counts[BLOCK_COUNT];
    uint8_t    barrier7[SPACING + 64 - ((sizeof(int) * BLOCK_COUNT) % 64)];
} locals;

int ram_read(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size) {
    memcpy(buffer, locals.ramdisk[block] + off, size);
    return 0;
}

int ram_prog(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size) {
    memcpy(locals.ramdisk[block] + off, buffer, size);
    return 0;
}

int ram_erase(const struct lfs_config* c, lfs_block_t block) {
    locals.erase_counts[block]++;
    memset(locals.ramdisk[block], 0xFF, c->block_size);
    return 0;
}

int ram_sync(const struct lfs_config* c) {
    return 0;
}

static const lfs_size_t read_size = sizeof(locals.read_buf);

// configuration of the filesystem is provided by this struct
const struct lfs_config cfg = {
    .read             = ram_read,
    .prog             = ram_prog,
    .erase            = ram_erase,
    .sync             = ram_sync,
    .name_max         = 32,
    .read_buffer      = locals.read_buf,
    .read_size        = BUFFER_SIZE,
    .prog_buffer      = locals.prog_buf,
    .prog_size        = 1,
    .lookahead_buffer = locals.lookahead_buf,
    .lookahead_size   = sizeof(locals.lookahead_buf),
    .block_size       = BLOCK_SIZE,
    .block_count      = BLOCK_COUNT,
    .block_cycles     = 5, // we go really low here despite docs mainly to ensure we level as much as possible
    .cache_size       = BUFFER_SIZE,
};

bool prep_test() {
    bool ret = true;
    memset(&locals, 0x00, sizeof(locals));
    memset(locals.barrier0, ' ', sizeof(locals.barrier0));
    memset(locals.barrier1, ' ', sizeof(locals.barrier1));
    memset(locals.barrier2, ' ', sizeof(locals.barrier2));
    memset(locals.barrier3, ' ', sizeof(locals.barrier3));
    memset(locals.barrier4, ' ', sizeof(locals.barrier4));
    memset(locals.barrier5, ' ', sizeof(locals.barrier5));
    memset(locals.barrier6, ' ', sizeof(locals.barrier6));
    memset(locals.barrier7, ' ', sizeof(locals.barrier7));
    memset(locals.ramdisk, 0xFF, sizeof(locals.ramdisk));

    // mount the filesystem
    int err = LFS_API_CALL(lfs_mount, &locals.lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        do {
            err = LFS_API_CALL(lfs_format, &locals.lfs, &cfg);
            if (err < 0) {
                ret = false;
                break;
            }
            err = LFS_API_CALL(lfs_mount, &locals.lfs, &cfg);
            if (err < 0) {
                ret = false;
                break;
            }
            if (sizeof(EEPROM_DIR_NAME) > 1) {
                err = LFS_API_CALL(lfs_mkdir, &locals.lfs, EEPROM_DIR_NAME);
                if (err < 0) {
                    ret = false;
                    break;
                }
            }
        } while (0);
    }

    // release any resources we were using
    LFS_API_CALL(lfs_unmount, &locals.lfs);

    return ret;
}

static long byte_counter = 0;

bool run_test(void) {
    bool ret = true;
    do {
        int err = LFS_API_CALL(lfs_mount, &locals.lfs, &cfg);
        if (err) {
            // reformat if we can't mount the filesystem
            // this should only happen on the first boot
            err = LFS_API_CALL(lfs_format, &locals.lfs, &cfg);
            if (err < 0) {
                ret = false;
                break;
            }
            err = LFS_API_CALL(lfs_mount, &locals.lfs, &cfg);
            if (err < 0) {
                ret = false;
                break;
            }
            if (sizeof(EEPROM_DIR_NAME) > 1) {
                err = LFS_API_CALL(lfs_mkdir, &locals.lfs, EEPROM_DIR_NAME);
                if (err < 0) {
                    ret = false;
                    break;
                }
            }
        }

        for (int i = 0; i < TOTAL_EEPROM_SLICES; ++i) {
            _Alignas(8) uint8_t value[BYTES_PER_SLICE] = {0};
            char                buf[16];

            sprintf(buf, EEPROM_DIR_NAME EEPROM_DIR_SEPARATOR EEPROM_FILE_PREFIX "%03d", i);
            err = LFS_API_CALL(lfs_file_open, &locals.lfs, &locals.file, buf, LFS_O_RDWR | LFS_O_CREAT);
            if (err < 0) {
                ret = false;
                break;
            }

            _Static_assert(BYTES_PER_SLICE % sizeof(uint64_t) == 0, "BYTES_PER_SLICE must be a multiple of 8");
            for (int j = 0; j < BYTES_PER_SLICE; j += sizeof(uint64_t))
                *(uint64_t*)&value[j] = splitmix64_next();

            err = LFS_API_CALL(lfs_file_write, &locals.lfs, &locals.file, value, BYTES_PER_SLICE);
            if (err < 0) {
                ret = false;
                break;
            }

            byte_counter += BYTES_PER_SLICE;

            err = LFS_API_CALL(lfs_file_close, &locals.lfs, &locals.file);
            if (err < 0) {
                ret = false;
                break;
            }
        }
    } while (0);

    // release any resources we were using
    LFS_API_CALL(lfs_unmount, &locals.lfs);

    return ret;
}

int main(void) {
    prep_test();

    long i = 0;
    while (true) {
        if (!run_test()) {
            break;
        }
        ++i;

        bool triggered = false;

        const int print_freq  = 1000;

        if (i % print_freq == 0) {
            printf("After %6ld iterations:", i);
        }
        for (int j = 0; j < sizeof(locals.erase_counts) / sizeof(locals.erase_counts[0]); ++j) {
            if (i % print_freq == 0) {
                printf("%s %5d", j == 0 ? "" : ",", locals.erase_counts[j]);
            }

            if (locals.erase_counts[j] >= erase_limit) triggered = true;
        }
        if (i % print_freq == 0) {
            printf("\n");
        }

        if (triggered) {
            printf("After %6ld iterations:", i);
            for (int j = 0; j < sizeof(locals.erase_counts) / sizeof(locals.erase_counts[0]); ++j) {
                printf("%s %5d", j == 0 ? "" : ",", locals.erase_counts[j]);
            }
            printf("\n\n");
            printf("Hit write endurance after %ld iterations, %ld bytes written\n", i, byte_counter);
            break;
        }
    }

    return 0;
}
