// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "stream.h"

int16_t mem_get(stream_t *stream) {
    memory_stream_t *s = (memory_stream_t *)stream;
    if (s->position >= s->length) return RLE_EOF;
    return s->buffer[s->position++];
}

bool mem_put(stream_t *stream, int16_t c) {
    memory_stream_t *s = (memory_stream_t *)stream;
    if (s->position >= s->length) return false;
    s->buffer[s->position++] = (uint16_t)c;
    return true;
}

int mem_seek(stream_t *stream, int offset, int origin) {
    memory_stream_t *s = (memory_stream_t *)stream;
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

int mem_tell(stream_t *stream) {
    memory_stream_t *s = (memory_stream_t *)stream;
    return s->position;
}

bool mem_is_eof(stream_t *stream) {
    memory_stream_t *s = (memory_stream_t *)stream;
    return s->position >= s->length;
}

memory_stream_t make_memory_stream(void STREAM_MEM_PTR *buffer, int length) {
    memory_stream_t stream = {
        .base =
            {
                .get    = mem_get,
                .put    = mem_put,
                .seek   = mem_seek,
                .tell   = mem_tell,
                .is_eof = mem_is_eof,
            },
        .buffer   = (uint8_t STREAM_MEM_PTR *)buffer,
        .length   = length,
        .position = 0,
    };
    return stream;
}

#ifdef RLE_HAS_FILE_IO
int16_t file_get(stream_t *stream) {
    file_stream_t *s = (file_stream_t *)stream;
    int            c = fgetc(s->file);
    if (c < 0 || feof(s->file)) return RLE_EOF;
    return (uint16_t)c;
}

bool file_put(stream_t *stream, int16_t c) {
    file_stream_t *s = (file_stream_t *)stream;
    return fputc(c, s->file) == c;
}

int file_seek(stream_t *stream, int offset, int origin) {
    file_stream_t *s = (file_stream_t *)stream;
    return fseek(s->file, offset, origin);
}

int file_tell(stream_t *stream) {
    file_stream_t *s = (file_stream_t *)stream;
    return (int)ftell(s->file);
}

bool file_is_eof(stream_t *stream) {
    file_stream_t *s = (file_stream_t *)stream;
    return (bool)feof(s->file);
}

file_stream_t make_file_stream(FILE *f) {
    file_stream_t stream = {
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
#endif  // RLE_HAS_FILE_IO
