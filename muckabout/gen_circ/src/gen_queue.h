// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*******************************************************************************

    Generic queue creator
    ---------------------

    This header is intended to be used to create queue types and corresponding
    API functions.

    Under normal circumstances, three defines can be specified before including
    this file, in order to configure a new queue type:

        #define GEN_QUEUE_NUM_ENTRIES 8
        #define GEN_QUEUE_VALUE_TYPE int
        #define GEN_QUEUE_NAMING_PREFIX event_

    This results in:

        An `int` queue type, of capacity `8`, with all types and APIs prefixed
        by `event_` -- the queue type itself resulting in `event_queue_t`:

        void event_queue_init(event_queue_t *queue);
        void* event_queue_data(event_queue_t *queue);
        size_t event_queue_size(void);
        bool event_queue_full(event_queue_t *queue);
        bool event_queue_empty(event_queue_t *queue);
        bool event_queue_push(event_queue_t *queue, const int *value);
        bool event_queue_pop(event_queue_t *queue, int *value);
        bool event_queue_peek(event_queue_t *queue, int *value);

    Users of the queue may also optionally implement the following APIs if they
    wish to provide support for locking:

        void event_queue_lock(event_queue_t *queue);
        void event_queue_unlock(event_queue_t *queue);

    Most APIs support a `_nolock()`-suffixed variant for each API if you wish to
    control concurrent access in calling code instead.

    Using the following define allows addition of extra fields into the queue,
    such as the inclusion of a mutex:

        #define GEN_QUEUE_EXTRA_FIELDS \
            my_mutex mutex;

    These will be included within the queue itself, but don't participate in the
    data definition for the purposes of calling `event_queue_data`.

    ---

    Configurables list:

        GEN_QUEUE_NAMING_PREFIX -- the prefix to add to the name of all
            datatypes and APIs. Setting this to `event_` results in an init call
            of `event_queue_init()`. Setting this to `priority_` results in an
            init call of `priority_queue_init()`.

        GEN_QUEUE_NUM_ENTRIES -- the number of entries that can exist in the
            queue at any one time.

        GEN_QUEUE_VALUE_TYPE -- the data type to store in the queue.

        GEN_QUEUE_EXTRA_FIELDS -- any extra field definitions to add to the
            queue itself.

        GEN_QUEUE_ENTRY_ATTRIBUTE -- an optional compiler attribute that can
            apply things like packing or alignment to each entry in the queue.

        GEN_QUEUE_DATA_ATTRIBUTE -- an optional compiler attribute that can
            apply things like packing or alignment to the data block.

        GEN_QUEUE_ATTRIBUTE -- an optional compiler attribute that can apply
            things like packing or alignment to the entire queue object.

        GEN_QUEUE_INDEX_TYPE -- the type used to index within the queue.
            Defaults to `int`, but can be dropped to something like `uint8_t` if
            the number of queue entries is small enough.

        GEN_QUEUE_GENERATION_TYPE -- the type used to store the generation
            within the queue. Defaults to `int`, but can be dropped to something
            like `uint8_t` if desired.

*******************************************************************************/

#ifndef GEN_QUEUE_NAMING_PREFIX
#    define GEN_QUEUE_NAMING_PREFIX
#endif

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

#ifndef GEN_QUEUE_DATA_ATTRIBUTE
#    define GEN_QUEUE_DATA_ATTRIBUTE
#endif

#ifndef GEN_QUEUE_INDEX_TYPE
#    define GEN_QUEUE_INDEX_TYPE uint8_t
#endif

#ifndef GEN_QUEUE_GENERATION_TYPE
#    define GEN_QUEUE_GENERATION_TYPE uint8_t
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

typedef struct GEN_QUEUE_ENTRY_T {
    GEN_QUEUE_VALUE_TYPE      value;
    GEN_QUEUE_GENERATION_TYPE generation;
} GEN_QUEUE_ENTRY_T GEN_QUEUE_ENTRY_ATTRIBUTE;

typedef struct GEN_QUEUE_T {
    struct {
        GEN_QUEUE_ENTRY_T         items[GEN_QUEUE_NUM_ENTRIES];
        GEN_QUEUE_INDEX_TYPE      write_idx;
        GEN_QUEUE_INDEX_TYPE      read_idx;
        GEN_QUEUE_GENERATION_TYPE generation;
    } data GEN_QUEUE_DATA_ATTRIBUTE;
    GEN_QUEUE_EXTRA_FIELDS;
} GEN_QUEUE_T GEN_QUEUE_ATTRIBUTE;

#undef GEN_QUEUE_LOCK
#define GEN_QUEUE_LOCK GEN_QUEUE_PREFIX_NAME(queue_lock)
extern void GEN_QUEUE_LOCK(GEN_QUEUE_T *queue) __attribute__((weak));

#undef GEN_QUEUE_UNLOCK
#define GEN_QUEUE_UNLOCK GEN_QUEUE_PREFIX_NAME(queue_unlock)
extern void GEN_QUEUE_UNLOCK(GEN_QUEUE_T *queue) __attribute__((weak));

#undef GEN_QUEUE_INIT
#define GEN_QUEUE_INIT GEN_QUEUE_PREFIX_NAME(queue_init)
void GEN_QUEUE_INIT(GEN_QUEUE_T *queue);

#undef GEN_QUEUE_DATA
#define GEN_QUEUE_DATA GEN_QUEUE_PREFIX_NAME(queue_data)
void *GEN_QUEUE_DATA(GEN_QUEUE_T *queue);

#undef GEN_QUEUE_SIZE
#define GEN_QUEUE_SIZE GEN_QUEUE_PREFIX_NAME(queue_size)
size_t GEN_QUEUE_SIZE(void);

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

void *GEN_QUEUE_DATA(GEN_QUEUE_T *queue) {
    return &queue->data;
}

size_t GEN_QUEUE_SIZE(void) {
    return (sizeof(((GEN_QUEUE_T *)NULL)->data));
}

bool GEN_QUEUE_FULL_NOLOCK(GEN_QUEUE_T *queue) {
    return (queue->data.write_idx % GEN_QUEUE_NUM_ENTRIES) == ((queue->data.read_idx + GEN_QUEUE_NUM_ENTRIES - 1) % GEN_QUEUE_NUM_ENTRIES);
}

bool GEN_QUEUE_FULL(GEN_QUEUE_T *queue) {
    GEN_QUEUE_LOCK_WRAP(queue);
    bool r = GEN_QUEUE_FULL_NOLOCK(queue);
    GEN_QUEUE_UNLOCK_WRAP(queue);
    return r;
}

bool GEN_QUEUE_EMPTY_NOLOCK(GEN_QUEUE_T *queue) {
    return (queue->data.write_idx % GEN_QUEUE_NUM_ENTRIES) == (queue->data.read_idx % GEN_QUEUE_NUM_ENTRIES);
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

    queue->data.items[queue->data.write_idx].generation = queue->data.generation;
    memcpy(&queue->data.items[queue->data.write_idx].value, entry, sizeof(GEN_QUEUE_VALUE_TYPE));

    ++queue->data.write_idx;
    if (queue->data.write_idx >= GEN_QUEUE_NUM_ENTRIES) {
        queue->data.write_idx = 0;
        ++queue->data.generation;
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

    memcpy(entry, &queue->data.items[queue->data.read_idx].value, sizeof(GEN_QUEUE_VALUE_TYPE));
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

    ++queue->data.read_idx;
    if (queue->data.read_idx >= GEN_QUEUE_NUM_ENTRIES) {
        queue->data.read_idx = 0;
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
#    undef GEN_QUEUE_NO_IMPL
#    undef GEN_QUEUE_NO_CLEANUP
#    undef GEN_QUEUE_NAMING_PREFIX
#    undef GEN_QUEUE_NUM_ENTRIES
#    undef GEN_QUEUE_VALUE_TYPE
#    undef GEN_QUEUE_EXTRA_FIELDS
#    undef GEN_QUEUE_ENTRY_ATTRIBUTE
#    undef GEN_QUEUE_ATTRIBUTE
#    undef GEN_QUEUE_DATA_ATTRIBUTE
#    undef GEN_QUEUE_INDEX_TYPE
#    undef GEN_QUEUE_GENERATION_TYPE
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
#    undef GEN_QUEUE_DATA
#    undef GEN_QUEUE_SIZE
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