// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifndef GEN_QUEUE_NUM_ENTRIES
#    error "GEN_QUEUE_NUM_ENTRIES is not defined before including gen_queue.h"
#endif

#ifndef GEN_QUEUE_VALUE_TYPE
#    error "GEN_QUEUE_VALUE_TYPE is not defined before including gen_queue.h"
#endif

#ifndef GEN_QUEUE_EXTRA_FIELDS
#    define GEN_QUEUE_EXTRA_FIELDS
#endif

#ifndef GEN_QUEUE_ENTRY_ATTRIBUTE
#    define GEN_QUEUE_ENTRY_ATTRIBUTE
#endif

#ifndef GEN_QUEUE_ATTRIBUTE
#    define GEN_QUEUE_ATTRIBUTE
#endif

#undef GEN_QUEUE_CONCAT
#undef GEN_QUEUE_CONCAT2
#define GEN_QUEUE_CONCAT(a, b) GEN_QUEUE_CONCAT2(a, b)
#define GEN_QUEUE_CONCAT2(a, b) a##b

#undef GEN_QUEUE_PREFIX_NAME
#define GEN_QUEUE_PREFIX_NAME(a) GEN_QUEUE_CONCAT(GEN_QUEUE_NAMING_PREFIX, a)

#undef GEN_QUEUE_ENTRY_T
#define GEN_QUEUE_ENTRY_T GEN_QUEUE_PREFIX_NAME(queue_entry_t)

#undef GEN_QUEUE_T
#define GEN_QUEUE_T GEN_QUEUE_PREFIX_NAME(queue_t)

typedef struct GEN_QUEUE_ENTRY_T GEN_QUEUE_ENTRY_ATTRIBUTE {
    uint8_t              generation;
    GEN_QUEUE_VALUE_TYPE value;
} GEN_QUEUE_ENTRY_T;

typedef struct GEN_QUEUE_T GEN_QUEUE_ATTRIBUTE {
    GEN_QUEUE_ENTRY_T items[GEN_QUEUE_NUM_ENTRIES];
    int               write_idx;
    int               read_idx;
    uint8_t           generation;
    GEN_QUEUE_EXTRA_FIELDS;
} GEN_QUEUE_T;

#undef GEN_QUEUE_LOCK
#define GEN_QUEUE_LOCK GEN_QUEUE_PREFIX_NAME(queue_lock)
extern void GEN_QUEUE_LOCK(GEN_QUEUE_T *queue) __attribute__((weak));

#undef GEN_QUEUE_UNLOCK
#define GEN_QUEUE_UNLOCK GEN_QUEUE_PREFIX_NAME(queue_unlock)
extern void GEN_QUEUE_UNLOCK(GEN_QUEUE_T *queue) __attribute__((weak));

#undef GEN_QUEUE_INIT
#define GEN_QUEUE_INIT GEN_QUEUE_PREFIX_NAME(queue_init)
void GEN_QUEUE_INIT(GEN_QUEUE_T *queue);

#undef GEN_QUEUE_FULL_NOLOCK
#define GEN_QUEUE_FULL_NOLOCK GEN_QUEUE_PREFIX_NAME(queue_full_nolock)
bool GEN_QUEUE_FULL_NOLOCK(GEN_QUEUE_T *queue);

#undef GEN_QUEUE_FULL
#define GEN_QUEUE_FULL GEN_QUEUE_PREFIX_NAME(queue_full)
bool GEN_QUEUE_FULL(GEN_QUEUE_T *queue);

#undef GEN_QUEUE_EMPTY_NOLOCK
#define GEN_QUEUE_EMPTY_NOLOCK GEN_QUEUE_PREFIX_NAME(queue_empty_nolock)
bool GEN_QUEUE_EMPTY_NOLOCK(GEN_QUEUE_T *queue);

#undef GEN_QUEUE_EMPTY
#define GEN_QUEUE_EMPTY GEN_QUEUE_PREFIX_NAME(queue_empty)
bool GEN_QUEUE_EMPTY(GEN_QUEUE_T *queue);

#undef GEN_QUEUE_PUSH_NOLOCK
#define GEN_QUEUE_PUSH_NOLOCK GEN_QUEUE_PREFIX_NAME(queue_push_nolock)
bool GEN_QUEUE_PUSH_NOLOCK(GEN_QUEUE_T *queue, const GEN_QUEUE_VALUE_TYPE *entry);

#undef GEN_QUEUE_PUSH
#define GEN_QUEUE_PUSH GEN_QUEUE_PREFIX_NAME(queue_push)
bool GEN_QUEUE_PUSH(GEN_QUEUE_T *queue, const GEN_QUEUE_VALUE_TYPE *entry);

#undef GEN_QUEUE_PEEK_NOLOCK
#define GEN_QUEUE_PEEK_NOLOCK GEN_QUEUE_PREFIX_NAME(queue_peek_nolock)
bool GEN_QUEUE_PEEK_NOLOCK(GEN_QUEUE_T *queue, GEN_QUEUE_VALUE_TYPE *entry);

#undef GEN_QUEUE_PEEK
#define GEN_QUEUE_PEEK GEN_QUEUE_PREFIX_NAME(queue_peek)
bool GEN_QUEUE_PEEK(GEN_QUEUE_T *queue, GEN_QUEUE_VALUE_TYPE *entry);

#undef GEN_QUEUE_POP_NOLOCK
#define GEN_QUEUE_POP_NOLOCK GEN_QUEUE_PREFIX_NAME(queue_pop_nolock)
bool GEN_QUEUE_POP_NOLOCK(GEN_QUEUE_T *queue, GEN_QUEUE_VALUE_TYPE *entry);

#undef GEN_QUEUE_POP
#define GEN_QUEUE_POP GEN_QUEUE_PREFIX_NAME(queue_pop)
bool GEN_QUEUE_POP(GEN_QUEUE_T *queue, GEN_QUEUE_VALUE_TYPE *entry);

#ifndef GEN_QUEUE_NO_IMPL

#    define GEN_QUEUE_LOCK_WRAP(queue) \
        do {                           \
            if (GEN_QUEUE_LOCK) {      \
                GEN_QUEUE_LOCK(queue); \
            }                          \
        } while (0)

#    define GEN_QUEUE_UNLOCK_WRAP(queue) \
        do {                             \
            if (GEN_QUEUE_UNLOCK) {      \
                GEN_QUEUE_UNLOCK(queue); \
            }                            \
        } while (0)

void GEN_QUEUE_INIT(GEN_QUEUE_T *queue) {
    memset(queue, 0, sizeof(GEN_QUEUE_T));
}

bool GEN_QUEUE_FULL_NOLOCK(GEN_QUEUE_T *queue) {
    return (queue->write_idx % GEN_QUEUE_NUM_ENTRIES) == ((queue->read_idx + GEN_QUEUE_NUM_ENTRIES - 1) % GEN_QUEUE_NUM_ENTRIES);
}

bool GEN_QUEUE_FULL(GEN_QUEUE_T *queue) {
    GEN_QUEUE_LOCK_WRAP(queue);
    bool r = GEN_QUEUE_FULL_NOLOCK(queue);
    GEN_QUEUE_UNLOCK_WRAP(queue);
    return r;
}

bool GEN_QUEUE_EMPTY_NOLOCK(GEN_QUEUE_T *queue) {
    return (queue->write_idx % GEN_QUEUE_NUM_ENTRIES) == (queue->read_idx % GEN_QUEUE_NUM_ENTRIES);
}

bool GEN_QUEUE_EMPTY(GEN_QUEUE_T *queue) {
    GEN_QUEUE_LOCK_WRAP(queue);
    bool r = GEN_QUEUE_EMPTY_NOLOCK(queue);
    GEN_QUEUE_UNLOCK_WRAP(queue);
    return r;
}

bool GEN_QUEUE_PUSH_NOLOCK(GEN_QUEUE_T *queue, const GEN_QUEUE_VALUE_TYPE *entry) {
    if (GEN_QUEUE_FULL_NOLOCK(queue)) {
        return false;
    }

    queue->items[queue->write_idx].generation = queue->generation;
    memcpy(&queue->items[queue->write_idx].value, entry, sizeof(GEN_QUEUE_VALUE_TYPE));

    ++queue->write_idx;
    if (queue->write_idx >= GEN_QUEUE_NUM_ENTRIES) {
        queue->write_idx = 0;
        ++queue->generation;
    }
    return true;
}

bool GEN_QUEUE_PUSH(GEN_QUEUE_T *queue, const GEN_QUEUE_VALUE_TYPE *entry) {
    GEN_QUEUE_LOCK_WRAP(queue);
    bool r = GEN_QUEUE_PUSH_NOLOCK(queue, entry);
    GEN_QUEUE_UNLOCK_WRAP(queue);
    return r;
}

bool GEN_QUEUE_PEEK_NOLOCK(GEN_QUEUE_T *queue, GEN_QUEUE_VALUE_TYPE *entry) {
    if (GEN_QUEUE_EMPTY_NOLOCK(queue)) {
        return false;
    }

    memcpy(entry, &queue->items[queue->read_idx].value, sizeof(GEN_QUEUE_VALUE_TYPE));
    return true;
}

bool GEN_QUEUE_PEEK(GEN_QUEUE_T *queue, GEN_QUEUE_VALUE_TYPE *entry) {
    GEN_QUEUE_LOCK_WRAP(queue);
    bool r = GEN_QUEUE_PEEK_NOLOCK(queue, entry);
    GEN_QUEUE_UNLOCK_WRAP(queue);
    return r;
}

bool GEN_QUEUE_POP_NOLOCK(GEN_QUEUE_T *queue, GEN_QUEUE_VALUE_TYPE *entry) {
    if (!GEN_QUEUE_PEEK_NOLOCK(queue, entry)) {
        return false;
    }

    ++queue->read_idx;
    if (queue->read_idx >= GEN_QUEUE_NUM_ENTRIES) {
        queue->read_idx = 0;
    }
    return true;
}

bool GEN_QUEUE_POP(GEN_QUEUE_T *queue, GEN_QUEUE_VALUE_TYPE *entry) {
    GEN_QUEUE_LOCK_WRAP(queue);
    bool r = GEN_QUEUE_POP_NOLOCK(queue, entry);
    GEN_QUEUE_UNLOCK_WRAP(queue);
    return r;
}

#endif // GEN_QUEUE_NO_IMPL

#ifndef GEN_QUEUE_NO_CLEANUP
// Inputs
#    undef GEN_QUEUE_NUM_ENTRIES
#    undef GEN_QUEUE_VALUE_TYPE
#    undef GEN_QUEUE_NAMING_PREFIX
#    undef GEN_QUEUE_EXTRA_FIELDS
#    undef GEN_QUEUE_NO_IMPL
#    undef GEN_QUEUE_NO_CLEANUP
// Helpers
#    undef GEN_QUEUE_CONCAT
#    undef GEN_QUEUE_CONCAT2
#    undef GEN_QUEUE_PREFIX_NAME
#    undef GEN_QUEUE_LOCK_WRAP
#    undef GEN_QUEUE_UNLOCK_WRAP
// API
#    undef GEN_QUEUE_ENTRY_T
#    undef GEN_QUEUE_T
#    undef GEN_QUEUE_LOCK
#    undef GEN_QUEUE_UNLOCK
#    undef GEN_QUEUE_LOCK_WRAP
#    undef GEN_QUEUE_UNLOCK_WRAP
#    undef GEN_QUEUE_INIT
#    undef GEN_QUEUE_FULL_NOLOCK
#    undef GEN_QUEUE_FULL
#    undef GEN_QUEUE_EMPTY_NOLOCK
#    undef GEN_QUEUE_EMPTY
#    undef GEN_QUEUE_PUSH_NOLOCK
#    undef GEN_QUEUE_PUSH
#    undef GEN_QUEUE_PEEK_NOLOCK
#    undef GEN_QUEUE_PEEK
#    undef GEN_QUEUE_POP_NOLOCK
#    undef GEN_QUEUE_POP
#endif // GEN_QUEUE_NO_CLEANUP