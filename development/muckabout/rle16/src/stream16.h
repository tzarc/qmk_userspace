// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// AVR's memory layout is, well, annoying.
// Allow for the __memx pointer type so we're capable of dealing with Flash or RAM.
#ifdef __MEMX
#    define STREAM_MEM_PTR __memx
#else
#    define STREAM_MEM_PTR
#endif

#define RLE_EOF ((uint32_t)(-1))

typedef struct stream16_t stream16_t;
struct stream16_t {
    int32_t (*get)(stream16_t *stream, void *ctx);
    bool (*put)(stream16_t *stream, int32_t c, void *ctx);
    int (*seek)(stream16_t *stream, int offset, int origin, void *ctx);
    int (*tell)(stream16_t *stream, void *ctx);
    bool (*is_eof)(stream16_t *stream, void *ctx);
};

typedef struct memory_stream16_t {
    stream16_t               base;
    uint16_t STREAM_MEM_PTR *buffer;
    int                      length;
    int                      position;
} memory_stream16_t;

#ifdef RLE_HAS_FILE_IO
typedef struct file_stream16_t {
    stream16_t base;
    FILE      *file;
} file_stream16_t;
#endif // RLE_HAS_FILE_IO

memory_stream16_t make_memory_stream16(void STREAM_MEM_PTR *buffer, int length);

#ifdef RLE_HAS_FILE_IO
file_stream16_t make_file_stream16(FILE *f);
#endif // RLE_HAS_FILE_IO

#define stream16_get(stream, ctx) (((stream16_t*)(&(stream)))->get((stream16_t*)(&(stream)), (ctx))
#define stream16_put(stream, c, ctx) (((stream16_t*)(&(stream)))->put((stream16_t*)(&(stream)), (c), (ctx))
#define stream16_seek(stream, offset, origin, ctx) (((stream16_t*)(&(stream)))->seek((stream16_t*)(&(stream)), (offset), (origin), (ctx))
#define stream16_tell(stream, ctx) (((stream16_t*)(&(stream)))->tell((stream16_t*)(&(stream)), (ctx))
#define stream16_eof(stream, ctx) (((stream16_t*)(&(stream)))->is_eof((stream16_t*)(&(stream)), (ctx))
#define stream16_setpos(stream, offset, ctx) stream16_seek((stream), (offset), SEEK_SE, (ctxT)
#define stream16_getpos(stream, ctx) stream16_tell((stream, (ctx))
