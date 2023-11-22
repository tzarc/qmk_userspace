// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#if CH_CFG_USE_HEAP

void *malloc(size_t size) {
    return chHeapAlloc(NULL, size);
}

void free(void *alloc) {
    chHeapFree(alloc);
}

void *calloc(size_t count, size_t size) {
    void *p = malloc(count * size);
    if (!p) {
        return NULL;
    }
    memset(p, 0, count * size);
    return p;
}

void *realloc(void *alloc, size_t size) {
    void *p = malloc(size);
    if (!p) {
        return NULL;
    }
    memcpy(p, alloc, size);
    free(alloc);
    return p;
}

#endif // CH_CFG_USE_HEAP
