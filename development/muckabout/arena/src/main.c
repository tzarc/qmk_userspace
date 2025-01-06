// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_ALLOCATION_ALIGNMENT 8

#define max(a, b)               \
    ({                          \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a > _b ? _a : _b;      \
    })

#define align_up(n, align) ((((n) + (align) - 1) / (align)) * (align))

typedef struct arena_link_t {
    uint32_t n_bytes;
    uint16_t prev_slot;
    uint16_t next_slot;
} arena_link_t;

_Static_assert(sizeof(arena_link_t) % 4 == 0, "arena_link_t must be 4-byte aligned");

typedef struct arena_t {
    uint8_t *backing;
    size_t   n_bytes;
} arena_t;

static inline void *arena_offset_to_ptr(arena_t *a, size_t offset) {
    uint8_t *p = a->backing + offset;
    if (p < a->backing || p >= a->backing + a->n_bytes) {
        return NULL;
    }
    return p;
}

static inline size_t arena_ptr_to_offset(arena_t *a, void *ptr) {
    uint8_t *p = (uint8_t *)ptr;
    if (p < a->backing || p >= a->backing + a->n_bytes) {
        return SIZE_MAX;
    }
    return p - a->backing;
}

static inline size_t arena_slot_to_offset(arena_t *a, size_t slot) {
    return slot * ARENA_ALLOCATION_ALIGNMENT;
}

static inline size_t arena_offset_to_slot(arena_t *a, size_t offset) {
    return offset / ARENA_ALLOCATION_ALIGNMENT;
}

static inline void *arena_slot_to_ptr(arena_t *a, size_t slot) {
    return arena_offset_to_ptr(a, arena_slot_to_offset(a, slot));
}

static inline size_t arena_ptr_to_slot(arena_t *a, void *ptr) {
    return arena_offset_to_slot(a, arena_ptr_to_offset(a, ptr));
}

static inline arena_link_t *arena_link_prev(arena_t *a, arena_link_t *link) {
    return (arena_link_t *)arena_slot_to_ptr(a, link->prev_slot);
}

static inline arena_link_t *arena_link_next(arena_t *a, arena_link_t *link) {
    return (arena_link_t *)arena_slot_to_ptr(a, link->next_slot);
}

static inline arena_link_t *arena_link_head(arena_t *a) {
    return (arena_link_t *)a->backing;
}

void arena_init(arena_t *a, uint8_t *backing, size_t n_bytes) {
    a->backing = backing;
    a->n_bytes = n_bytes;

    void arena_clear(arena_t * a);
    arena_clear(a);
}

static inline arena_link_t *arena_link_malloc_internal(arena_t *a, arena_link_t *link, size_t n_bytes) {
    arena_link_t *new_next = (arena_link_t *)((uint8_t *)link + n_bytes);
    new_next->n_bytes      = link->n_bytes - n_bytes;
    new_next->prev_slot    = arena_ptr_to_slot(a, link);
    new_next->next_slot    = link->next_slot;
    link->next_slot        = arena_ptr_to_slot(a, new_next);
    link->n_bytes          = n_bytes;
    return link;
}

static inline arena_link_t *arena_malloc_internal(arena_t *a, size_t n_bytes) {
    size_t        n_bytes_aligned = align_up(sizeof(arena_link_t), ARENA_ALLOCATION_ALIGNMENT) + align_up(n_bytes, ARENA_ALLOCATION_ALIGNMENT);
    arena_link_t *link            = arena_link_head(a);
    while (link && link->n_bytes < n_bytes_aligned) {
        link = arena_link_next(a, link);
    }
    if (link == NULL) {
        return NULL;
    }
    return arena_link_malloc_internal(a, link, n_bytes_aligned);
}

void *arena_malloc(arena_t *a, size_t n_bytes) {
    arena_link_t *link = arena_malloc_internal(a, n_bytes);
    if (link == NULL) {
        return NULL;
    }
    return (uint8_t *)link + sizeof(arena_link_t);
}

void arena_free(arena_t *a, void *ptr) {
    arena_link_t *link = (arena_link_t *)((uint8_t *)ptr - sizeof(arena_link_t));
    arena_link_t *prev = arena_link_prev(a, link);
    arena_link_t *next = arena_link_next(a, link);
    if (prev) {
        prev->next_slot = link->next_slot;
    }
    if (next) {
        next->prev_slot = link->prev_slot;
    }
}

void arena_clear(arena_t *a) {
    memset(a->backing, 0, a->n_bytes);
    arena_link_t *link = arena_link_head(a);
    link->n_bytes      = a->n_bytes;
    link->prev_slot    = arena_ptr_to_slot(a, NULL);
    link->next_slot    = arena_ptr_to_slot(a, NULL);
}

#include <stdio.h>
int main(int argc, const char *argv[]) {
    uint8_t backing[1024];
    arena_t a;
    arena_init(&a, backing, sizeof(backing));
    void *p = arena_malloc(&a, 5);
    arena_free(&a, p);
    return 0;
}
