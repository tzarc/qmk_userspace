// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "rle16.h"

bool rle16_decode(stream16_t *in_stream, stream16_t *out_stream, void *ctx) {
    int32_t (*get)(stream16_t *stream, void *ctx)         = in_stream->get;
    bool (*put)(stream16_t *stream, int32_t c, void *ctx) = out_stream->put;
    int32_t c;
    size_t  i, cnt;
    while (1) {
        c = get(in_stream, ctx);
        if (c == RLE_EOF) break;
        if (c >= 32768) {
            cnt = c - 32767;
            for (i = 0; i < cnt; i++) {
                c = get(in_stream, ctx);
                if (c == RLE_EOF) return false;
                if (!put(out_stream, c, ctx)) return false;
            }
        } else {
            cnt = c;
            c   = get(in_stream, ctx);
            if (c == RLE_EOF) return false;
            for (i = 0; i < cnt; i++) {
                if (!put(out_stream, c, ctx)) return false;
            }
        }
    }
    return true;
}

#ifdef RLE_ENCODER
static bool rle16_append_words(stream16_t *out_stream, uint16_t *buf, size_t len, void *ctx) {
    bool (*put)(stream16_t *stream, int32_t c, void *ctx) = out_stream->put;
    int i;
    if (!put(out_stream, (int32_t)(32767 + len), ctx)) return false;
    for (i = 0; i < len; i++) {
        if (!put(out_stream, buf[i], ctx)) return false;
    }
    return true;
}

bool rle16_encode(stream16_t *in_stream, stream16_t *out_stream, void *ctx) {
    int32_t (*get)(stream16_t *stream, void *ctx)         = in_stream->get;
    bool (*put)(stream16_t *stream, int32_t c, void *ctx) = out_stream->put;
    uint16_t buf[65536];
    size_t   len    = 0;
    bool     repeat = false;
    bool     end    = false;
    int32_t  c;

    while (!end) {
        end = ((c = get(in_stream, ctx)) == RLE_EOF);
        if (!end) {
            buf[len++] = (uint16_t)c;
            if (len <= 1) continue;
        }

        if (repeat) {
            if (buf[len - 1] != buf[len - 2]) repeat = false;
            if (!repeat || len == 32768 || end) {
                if (!put(out_stream, (int32_t)(end ? len : len - 1), ctx)) return false;
                if (!put(out_stream, buf[0], ctx)) return false;
                buf[0] = buf[len - 1];
                len    = 1;
                repeat = false;
            }
        } else {
            if (buf[len - 1] == buf[len - 2]) {
                repeat = true;
                if (len > 2) {
                    if (!rle16_append_words(out_stream, buf, (len - 2), ctx)) return false;
                    buf[0] = buf[1] = buf[len - 1];
                    len             = 2;
                }
                continue;
            }
            if (len == 32768 || end) {
                if (!rle16_append_words(out_stream, buf, len, ctx)) return false;
                len    = 0;
                repeat = false;
            }
        }
    }
    return true;
}
#endif // RLE_ENCODER
