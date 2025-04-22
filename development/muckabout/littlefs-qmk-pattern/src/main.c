// Copyright 2022-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <getopt.h>

#include "lfs.h"

#define LFS_API_CALL(api, ...)                  \
    ({                                          \
        int ret = api(__VA_ARGS__);             \
        if (ret < 0) {                          \
            printf(#api " returned %d\n", ret); \
        }                                       \
        ret;                                    \
    })

typedef struct testrun_t {
    size_t    block_count;
    uint8_t **ram_disk;
    uint8_t  *lookahead_buf;
    uint8_t  *read_buf;
    uint8_t  *prog_buf;

    lfs_t      lfs;
    lfs_file_t file;
    int       *erase_counts;

    bool   endurance_hit;
    int    erase_limit;
    int    max_erases;
    size_t total_bytes_written;
} testrun_t;

static testrun_t locals;

int ram_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
    memcpy(buffer, locals.ram_disk[block] + off, size);
    return 0;
}

int ram_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) {
    memcpy(locals.ram_disk[block] + off, buffer, size);
    return 0;
}

int ram_erase(const struct lfs_config *c, lfs_block_t block) {
    locals.erase_counts[block]++;
    memset(locals.ram_disk[block], 0xFF, c->block_size);
    if (locals.max_erases < locals.erase_counts[block]) {
        locals.max_erases = locals.erase_counts[block];
    }
    if (locals.erase_counts[block] >= locals.erase_limit) {
        locals.endurance_hit = true;
    }
    return 0;
}

int ram_sync(const struct lfs_config *c) {
    return 0;
}

void free_buffers(void) {
    for (size_t i = 0; i < locals.block_count; ++i) {
        if (locals.ram_disk[i] != NULL) {
            free(locals.ram_disk[i]);
        }
    }
    if (locals.ram_disk != NULL) {
        free(locals.ram_disk);
    }
    locals.ram_disk    = NULL;
    locals.block_count = 0;

    if (locals.lookahead_buf != NULL) {
        free(locals.lookahead_buf);
    }
    locals.lookahead_buf = NULL;

    if (locals.read_buf != NULL) {
        free(locals.read_buf);
    }
    locals.read_buf = NULL;

    if (locals.prog_buf != NULL) {
        free(locals.prog_buf);
    }
    locals.prog_buf = NULL;

    if (locals.erase_counts != NULL) {
        free(locals.erase_counts);
    }
    locals.erase_counts = NULL;
}

void prepare_buffers(size_t block_count, size_t block_size, size_t lookahead_size, size_t buffer_size) {
    free_buffers();
    memset(&locals, 0, sizeof(locals));
    locals.block_count = block_count;
    locals.ram_disk    = calloc(sizeof(uint8_t *), block_count);
    for (size_t i = 0; i < block_count; ++i) {
        locals.ram_disk[i] = malloc(block_size);
        memset(locals.ram_disk[i], 0xFF, block_size);
    }
    locals.lookahead_buf = calloc(1, lookahead_size);
    locals.read_buf      = calloc(1, buffer_size);
    locals.prog_buf      = calloc(1, buffer_size);
    locals.erase_counts  = calloc(sizeof(int), block_count);
}

struct lfs_config *prepare_config(int erase_limit, size_t block_count, size_t block_size, size_t lookahead_size, size_t buffer_size, uint8_t prog_size) {
    prepare_buffers(block_count, block_size, lookahead_size, buffer_size);
    locals.erase_limit = erase_limit;

    struct lfs_config *cfg = calloc(1, sizeof(struct lfs_config));

    *cfg = (struct lfs_config){
        .read             = ram_read,
        .prog             = ram_prog,
        .erase            = ram_erase,
        .sync             = ram_sync,
        .name_max         = 32,
        .read_buffer      = locals.read_buf,
        .read_size        = buffer_size,
        .prog_buffer      = locals.prog_buf,
        .prog_size        = prog_size,
        .lookahead_buffer = locals.lookahead_buf,
        .lookahead_size   = lookahead_size,
        .block_size       = block_size,
        .block_count      = block_count,
        .block_cycles     = 5,
        .cache_size       = buffer_size,
    };

    return cfg;
}

typedef struct qmk_file_pattern_t {
    const char *filename;
    size_t      length;
    size_t      write_freq;
    size_t      write_count;
} qmk_file_pattern_t;

// clang-format off
static const char eeprom_dir_name[] = "ee";
static const char layer_dir_name[] = "layers";
static qmk_file_pattern_t qmk_file_patterns[] = {
    {"ee/magic",            2,     1,    0},
    {"ee/debug",            2,    10,    0},
    {"ee/default_layer",    4,    20,    0},
    {"ee/keymap",           4,    40,    0},
    {"ee/keyboard",         4,    40,    0},
    {"ee/user",             4,  1000,    0},
    {"ee/handedness",       1,    10,    0},
    {"ee/keymap_hash",      4,    10,    0},
    {"ee/audio",            4,    10,    0},
    {"ee/unicodemode",      4,    10,    0},
    {"ee/backlight",        4,    10,    0},
    {"ee/stenomode",        1,    10,    0},
    {"ee/rgb_matrix",       8, 20000,    0},
    {"ee/haptic",           4,    40,    0},
    {"layers/key00",       17,    60,    0}, // 6 * 23 * 2
    {"layers/key01",        9,    20,    0}, // 6 * 23 * 2
    {"layers/key02",        9,    20,    0}, // 6 * 23 * 2
    {"layers/key03",        5,    10,    0}, // 6 * 23 * 2
    {"layers/key04",        5,    10,    0}, // 6 * 23 * 2
};
// clang-format on

static const size_t qmk_file_patterns_count = sizeof(qmk_file_patterns) / sizeof(qmk_file_patterns[0]);

qmk_file_pattern_t *get_pattern(int freq) {
    int freq_counter = 0;
    for (int i = 0; i < qmk_file_patterns_count; ++i) {
        freq_counter += qmk_file_patterns[i].write_freq;
        if (freq_counter >= freq) {
            return &qmk_file_patterns[i];
        }
    }
    return NULL;
}

qmk_file_pattern_t *get_random_pattern(int max_freq) {
    int freq = rand() % max_freq;
    return get_pattern(freq);
}

int main(int argc, char *argv[]) {
    setlocale(LC_NUMERIC, "");

    struct timespec ts;
    srand(clock_gettime(CLOCK_MONOTONIC, &ts) ? rand() : (unsigned)ts.tv_nsec);

    // 8kB EEPROM
    struct lfs_config *cfg = prepare_config(100000, 64, 128, 128, 128, 1);

    // 32kB EEPROM
    // struct lfs_config *cfg = prepare_config(100000, 256, 128, 128, 128, 1);

    // M95256 32kB EEPROM, 4mil writes
    // struct lfs_config *cfg = prepare_config(4000000, 256, 128, 128, 128, 1);

    // 1MB NOR Flash
    // struct lfs_config *cfg = prepare_config(10000, 256, 4096, 256, 256, 1);

    // 16MB NOR Flash
    // struct lfs_config *cfg = prepare_config(10000, 256*16, 4096, 256, 256, 1);

    if (cfg == NULL) {
        fprintf(stderr, "Failed to allocate memory for config\n");
        return 1;
    }

    int ret = 0;
    do {
        int err = LFS_API_CALL(lfs_format, &locals.lfs, cfg);
        if (err < 0) {
            ret = 1;
            break;
        }

        err = LFS_API_CALL(lfs_mount, &locals.lfs, cfg);
        if (err < 0) {
            ret = 1;
            break;
        }

        err = LFS_API_CALL(lfs_mkdir, &locals.lfs, eeprom_dir_name);
        if (err < 0) {
            ret = false;
            break;
        }

        err = LFS_API_CALL(lfs_mkdir, &locals.lfs, layer_dir_name);
        if (err < 0) {
            ret = false;
            break;
        }

        size_t total_freq_count = 0;
        for (int i = 0; i < qmk_file_patterns_count; ++i) {
            total_freq_count += qmk_file_patterns[i].write_freq;
        }

        size_t files_written = 0;
        while (!locals.endurance_hit) {
            char                buf[16];
            qmk_file_pattern_t *pattern = get_random_pattern(total_freq_count);

            union {
                uint32_t u32[2];
                uint8_t  u8[8];
            } d;
            d.u32[0] = rand();
            d.u32[1] = rand();

            err = LFS_API_CALL(lfs_file_open, &locals.lfs, &locals.file, pattern->filename, LFS_O_RDWR | LFS_O_CREAT);
            if (err < 0) {
                ret = false;
                break;
            }

            err = LFS_API_CALL(lfs_file_write, &locals.lfs, &locals.file, d.u8, pattern->length);
            if (err < 0) {
                ret = false;
                break;
            }

            err = LFS_API_CALL(lfs_file_close, &locals.lfs, &locals.file);
            if (err < 0) {
                ret = false;
                break;
            }

            files_written++;
            pattern->write_count++;
            locals.total_bytes_written += pattern->length;

            if (locals.endurance_hit || (files_written % 100000 == 0)) {
                printf("----------------------\n");
                printf("Files written: %'zu\n", files_written);
                printf("Total bytes written: %'zu\n", locals.total_bytes_written);
                printf("Max erases: %'d\n", locals.max_erases);
                printf("----------------------\n");
                for (int i = 0; i < qmk_file_patterns_count; ++i) {
                    printf("%s: %'zu writes\n", qmk_file_patterns[i].filename, qmk_file_patterns[i].write_count);
                }
            }
        }

    } while (0);

    free(cfg);
    return ret;
}
