// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "stream16.h"

int32_t mem_get(stream16_t* stream, void* ctx) {
    memory_stream16_t* s = (memory_stream16_t*)stream;
    if (s->position >= s->length) return RLE_EOF;
    return s->buffer[s->position++];
}

bool mem_put(stream16_t* stream, int32_t c, void* ctx) {
    memory_stream16_t* s = (memory_stream16_t*)stream;
    if (s->position >= s->length) return false;
    s->buffer[s->position++] = (uint32_t)c;
    return true;
}

int mem_seek(stream16_t* stream, int offset, int origin, void* ctx) {
    memory_stream16_t* s = (memory_stream16_t*)stream;
    switch (origin) {
        case SEEK_SET:
            s->position = offset;
            break;
        case SEEK_CUR:
            s->position += offset;
            break;
        case SEEK_END:
            s->position = s->length + offset;
            break;
    }
    if (s->position < 0) s->position = 0;
    if (s->position >= s->length) s->position = s->length;
    return 0;
}

int mem_tell(stream16_t* stream, void* ctx) {
    memory_stream16_t* s = (memory_stream16_t*)stream;
    return s->position;
}

bool mem_is_eof(stream16_t* stream, void* ctx) {
    memory_stream16_t* s = (memory_stream16_t*)stream;
    return s->position >= s->length;
}

memory_stream16_t make_memory_stream16(void STREAM_MEM_PTR* buffer, int length) {
    memory_stream16_t stream = {
        .base =
            {
                .get    = mem_get,
                .put    = mem_put,
                .seek   = mem_seek,
                .tell   = mem_tell,
                .is_eof = mem_is_eof,
            },
        .buffer   = (uint16_t STREAM_MEM_PTR*)buffer,
        .length   = length,
        .position = 0,
    };
    return stream;
}

#ifdef RLE_HAS_FILE_IO
int32_t file_get(stream16_t* stream, void* ctx) {
    file_stream16_t* s  = (file_stream16_t*)stream;
    int              c1 = fgetc(s->file);
    if (c1 < 0 || feof(s->file)) return RLE_EOF;
    int c2 = fgetc(s->file);
    if (c2 < 0 || feof(s->file)) return RLE_EOF;
    return (uint32_t)(c2 << 8) | (uint32_t)c1;
}

bool file_put(stream16_t* stream, int32_t c, void* ctx) {
    file_stream16_t* s  = (file_stream16_t*)stream;
    uint8_t          c1 = c & 0xFF;
    uint8_t          c2 = (c >> 8) & 0xFF;
    return fputc(c1, s->file) == c1 && fputc(c2, s->file) == c2;
}

int file_seek(stream16_t* stream, int offset, int origin, void* ctx) {
    file_stream16_t* s = (file_stream16_t*)stream;
    return fseek(s->file, offset * 2, origin);
}

int file_tell(stream16_t* stream, void* ctx) {
    file_stream16_t* s = (file_stream16_t*)stream;
    return ((int)ftell(s->file)) / 2;
}

bool file_is_eof(stream16_t* stream, void* ctx) {
    file_stream16_t* s = (file_stream16_t*)stream;
    return (bool)feof(s->file);
}

file_stream16_t make_file_stream16(FILE* f) {
    file_stream16_t stream = {
        .base =
            {
                .get    = file_get,
                .put    = file_put,
                .seek   = file_seek,
                .tell   = file_tell,
                .is_eof = file_is_eof,
            },
        .file = f,
    };
    return stream;
}
#endif // RLE_HAS_FILE_IO
