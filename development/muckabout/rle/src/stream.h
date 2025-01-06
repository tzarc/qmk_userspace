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

#define RLE_EOF ((int16_t)(-1))

typedef struct stream_t stream_t;
struct stream_t {
    int16_t (*get)(stream_t* stream);
    bool (*put)(stream_t* stream, int16_t c);
    int (*seek)(stream_t* stream, int offset, int origin);
    int (*tell)(stream_t* stream);
    bool (*is_eof)(stream_t* stream);
};

typedef struct memory_stream_t {
    stream_t                base;
    uint8_t STREAM_MEM_PTR* buffer;
    int                     length;
    int                     position;
} memory_stream_t;

#ifdef RLE_HAS_FILE_IO
typedef struct file_stream_t {
    stream_t base;
    FILE*    file;
} file_stream_t;
#endif // RLE_HAS_FILE_IO

memory_stream_t make_memory_stream(void STREAM_MEM_PTR* buffer, int length);

#ifdef RLE_HAS_FILE_IO
file_stream_t make_file_stream(FILE* f);
#endif // RLE_HAS_FILE_IO

#define stream_get(stream) (((stream_t*)(&(stream)))->get((stream_t*)(&(stream))))
#define stream_put(stream, c) (((stream_t*)(&(stream)))->put((stream_t*)(&(stream)), (c)))
#define stream_seek(stream, offset, origin) (((stream_t*)(&(stream)))->seek((stream_t*)(&(stream)), (offset), (origin)))
#define stream_tell(stream) (((stream_t*)(&(stream)))->tell((stream_t*)(&(stream))))
#define stream_eof(stream) (((stream_t*)(&(stream)))->is_eof((stream_t*)(&(stream))))
#define stream_setpos(stream, offset) stream_seek((stream), (offset), SEEK_SET)
#define stream_getpos(stream) stream_tell((stream))
