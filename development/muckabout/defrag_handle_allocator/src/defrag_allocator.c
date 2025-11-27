// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "defrag_allocator.h"
#include <string.h>
#include <assert.h>

// Helper macros for thread-safety callbacks
#define LOCK(alloc)                               \
    do {                                          \
        if ((alloc)->lock_fn) {                   \
            (alloc)->lock_fn((alloc)->user_data); \
        }                                         \
    } while (0)

#define UNLOCK(alloc)                               \
    do {                                            \
        if ((alloc)->unlock_fn) {                   \
            (alloc)->unlock_fn((alloc)->user_data); \
        }                                           \
    } while (0)

// Helper: Get pointer to handle table at start of buffer
static inline defrag_handle_entry_t *get_handles(defrag_allocator_t *allocator) {
    return (defrag_handle_entry_t *)allocator->buffer;
}

// Helper: Get specific handle entry by searching for handle ID
static inline defrag_handle_entry_t *get_handle_entry(defrag_allocator_t *allocator, defrag_handle_t handle) {
    if (handle == DEFRAG_INVALID_HANDLE) {
        return NULL;
    }

    defrag_handle_entry_t *handles = get_handles(allocator);
    for (uint32_t i = 0; i < allocator->handle_count; i++) {
        if (handles[i].handle_id == handle) {
            return &handles[i];
        }
    }
    return NULL;
}

// Helper: Get size of handle table
static inline uint32_t get_handle_table_size(defrag_allocator_t *allocator) {
    return allocator->handle_count * sizeof(defrag_handle_entry_t);
}

// Helper: Calculate the lowest offset used by allocations (grows from end backward)
static uint32_t calculate_alloc_start(defrag_allocator_t *allocator) {
    defrag_handle_entry_t *handles    = get_handles(allocator);
    uint32_t               min_offset = allocator->buffer_size;

    for (uint32_t i = 0; i < allocator->handle_count; i++) {
        if (DEFRAG_GET_SIZE(&handles[i]) > 0 && handles[i].offset < min_offset) {
            min_offset = handles[i].offset;
        }
    }
    return min_offset;
}

void defrag_allocator_init(defrag_allocator_t *allocator, void *buffer, uint32_t buffer_size) {
    assert(allocator != NULL);
    assert(buffer != NULL);
    assert(buffer_size >= sizeof(defrag_handle_entry_t));

    allocator->buffer         = (uint8_t *)buffer;
    allocator->buffer_size    = buffer_size;
    allocator->handle_count   = 0;
    allocator->next_handle_id = 1; // Start at 1 (0 is invalid)
    allocator->lock_fn        = NULL;
    allocator->unlock_fn      = NULL;
    allocator->user_data      = NULL;
}

void defrag_allocator_set_callbacks(defrag_allocator_t *allocator, void (*lock_fn)(void *), void (*unlock_fn)(void *), void *user_data) {
    assert(allocator != NULL);
    allocator->lock_fn   = lock_fn;
    allocator->unlock_fn = unlock_fn;
    allocator->user_data = user_data;
}

// Find next available handle ID with wraparound protection
// Returns DEFRAG_INVALID_HANDLE if all IDs are in use
static defrag_handle_t find_available_handle_id(defrag_allocator_t *allocator) {
    defrag_handle_entry_t *handles       = get_handles(allocator);
    defrag_handle_t        candidate     = allocator->next_handle_id;
    uint32_t               attempts      = 0;
    const uint32_t         max_handle_id = 65535; // 16-bit max

    while (attempts < max_handle_id) {
        // Wraparound: skip 0 (invalid) and wrap to 1
        if (candidate == 0 || candidate > max_handle_id) {
            candidate = 1;
        }

        // Check if this ID is already in use
        int in_use = 0;
        for (uint32_t i = 0; i < allocator->handle_count; i++) {
            if (handles[i].handle_id == candidate) {
                in_use = 1;
                break;
            }
        }

        if (!in_use) {
            // Found an available ID, update hint for next allocation
            allocator->next_handle_id = candidate + 1;
            return candidate;
        }

        candidate++;
        attempts++;
    }

    // All handle IDs exhausted (65535 concurrent handles in use)
    return DEFRAG_INVALID_HANDLE;
}

static defrag_handle_t allocate_handle(defrag_allocator_t *allocator) {
    // Check if we have space for another handle entry
    uint32_t new_handle_count = allocator->handle_count + 1;
    uint32_t new_table_size   = new_handle_count * sizeof(defrag_handle_entry_t);

    if (new_table_size >= allocator->buffer_size) {
        return DEFRAG_INVALID_HANDLE;
    }

    // Find an available handle ID
    defrag_handle_t handle = find_available_handle_id(allocator);
    if (handle == DEFRAG_INVALID_HANDLE) {
        return DEFRAG_INVALID_HANDLE;
    }

    // Add entry to the handle table
    defrag_handle_entry_t *handles             = get_handles(allocator);
    handles[allocator->handle_count].handle_id = handle;
    handles[allocator->handle_count].offset    = 0;
    DEFRAG_SET_SIZE(&handles[allocator->handle_count], 0);
    handles[allocator->handle_count].lock_count = 0;

    allocator->handle_count = new_handle_count;
    return handle;
}

static uint32_t calculate_used_bytes(defrag_allocator_t *allocator) {
    // Total used = handle table size + sum of all allocations
    uint32_t               handle_table_size = get_handle_table_size(allocator);
    uint32_t               alloc_total       = 0;
    defrag_handle_entry_t *handles           = get_handles(allocator);

    for (uint32_t i = 0; i < allocator->handle_count; i++) {
        alloc_total += DEFRAG_GET_SIZE(&handles[i]);
    }
    return handle_table_size + alloc_total;
}

defrag_handle_t defrag_allocator_alloc(defrag_allocator_t *allocator, uint32_t size) {
    assert(allocator != NULL);
    if (size == 0) {
        return DEFRAG_INVALID_HANDLE;
    }

    LOCK(allocator);

    // Allocate a handle (may grow the handle table)
    defrag_handle_t handle = allocate_handle(allocator);
    if (handle == DEFRAG_INVALID_HANDLE) {
        UNLOCK(allocator);
        return DEFRAG_INVALID_HANDLE;
    }

    // Calculate available space (two-ended: handles from start, allocations from end)
    uint32_t handle_table_end = get_handle_table_size(allocator);
    uint32_t alloc_start      = calculate_alloc_start(allocator);

    // Check if there's room between handle table and allocations
    if (alloc_start < handle_table_end + size) {
        // Not enough space - remove the handle entry we just added
        allocator->handle_count--;
        allocator->next_handle_id--;
        UNLOCK(allocator);
        return DEFRAG_INVALID_HANDLE;
    }

    // Place allocation at the lowest available offset (growing backward from end)
    uint32_t new_offset = alloc_start - size;

    // Setup the allocation
    defrag_handle_entry_t *entry = get_handle_entry(allocator, handle);
    entry->offset                = new_offset;
    DEFRAG_SET_SIZE(entry, size);
    entry->lock_count = 0;

    UNLOCK(allocator);
    return handle;
}

void *defrag_allocator_lock(defrag_allocator_t *allocator, defrag_handle_t handle) {
    assert(allocator != NULL);

    if (handle == DEFRAG_INVALID_HANDLE) {
        return NULL;
    }

    LOCK(allocator);

    defrag_handle_entry_t *entry = get_handle_entry(allocator, handle);
    if (entry == NULL) {
        UNLOCK(allocator);
        return NULL;
    }

    entry->lock_count++;
    void *ptr = allocator->buffer + entry->offset;

    UNLOCK(allocator);
    return ptr;
}

void defrag_allocator_unlock(defrag_allocator_t *allocator, defrag_handle_t handle) {
    assert(allocator != NULL);

    if (handle == DEFRAG_INVALID_HANDLE) {
        return;
    }

    LOCK(allocator);

    defrag_handle_entry_t *entry = get_handle_entry(allocator, handle);
    if (entry == NULL) {
        UNLOCK(allocator);
        return;
    }

    if (entry->lock_count > 0) {
        entry->lock_count--;
    }

    UNLOCK(allocator);
}

void defrag_allocator_free(defrag_allocator_t *allocator, defrag_handle_t handle) {
    assert(allocator != NULL);

    if (handle == DEFRAG_INVALID_HANDLE) {
        return;
    }

    LOCK(allocator);

    defrag_handle_entry_t *handles = get_handles(allocator);

    // Find the entry
    uint32_t entry_idx = 0;
    int      found     = 0;
    for (uint32_t i = 0; i < allocator->handle_count; i++) {
        if (handles[i].handle_id == handle) {
            entry_idx = i;
            found     = 1;
            break;
        }
    }

    if (!found) {
        UNLOCK(allocator);
        return;
    }

    // Basic validation: handle should be unlocked before freeing
    assert(handles[entry_idx].lock_count == 0);

    // Move last entry into vacated slot (order doesn't matter since we search by handle_id)
    // This is O(1) instead of O(n) like memmove would be
    if (entry_idx < allocator->handle_count - 1) {
        handles[entry_idx] = handles[allocator->handle_count - 1];
    }
    allocator->handle_count--;

    UNLOCK(allocator);
}

void defrag_allocator_defragment(defrag_allocator_t *allocator) {
    assert(allocator != NULL);

    LOCK(allocator);

    defrag_handle_entry_t *handles = get_handles(allocator);

    // Compact allocations toward the end of the buffer (growing backward)
    uint32_t write_offset = allocator->buffer_size;

    // Find where locked allocations start (they can't move)
    uint32_t locked_start = allocator->buffer_size;
    for (uint32_t i = 0; i < allocator->handle_count; i++) {
        if (handles[i].lock_count > 0 && handles[i].offset < locked_start) {
            locked_start = handles[i].offset;
        }
    }

    // Can't write past locked allocations
    if (locked_start < write_offset) {
        write_offset = locked_start;
    }

    // Move unlocked allocations from highest offset to lowest
    // This prevents overwriting data that hasn't been moved yet
    uint32_t total_moveable = 0;
    for (uint32_t i = 0; i < allocator->handle_count; i++) {
        if (handles[i].lock_count == 0) {
            total_moveable++;
        }
    }

    for (uint32_t moved = 0; moved < total_moveable; moved++) {
        // Find unmoved allocation with highest offset
        uint32_t highest_idx    = 0;
        uint32_t highest_offset = 0;
        int      found          = 0;

        for (uint32_t i = 0; i < allocator->handle_count; i++) {
            if (handles[i].lock_count == 0 && handles[i].offset < write_offset) {
                if (!found || handles[i].offset > highest_offset) {
                    highest_idx    = i;
                    highest_offset = handles[i].offset;
                    found          = 1;
                }
            }
        }

        if (!found) break;

        // Move this allocation
        uint32_t size       = DEFRAG_GET_SIZE(&handles[highest_idx]);
        uint32_t new_offset = write_offset - size;
        if (handles[highest_idx].offset != new_offset) {
            memmove(allocator->buffer + new_offset, allocator->buffer + handles[highest_idx].offset, size);
            handles[highest_idx].offset = new_offset;
        }

        write_offset = new_offset;
    }

    UNLOCK(allocator);
}

void defrag_allocator_stats(defrag_allocator_t *allocator, uint32_t *used, uint32_t *capacity, uint32_t *handles_used, uint32_t *handles_capacity) {
    assert(allocator != NULL);

    LOCK(allocator);

    if (used != NULL) {
        *used = calculate_used_bytes(allocator);
    }

    if (capacity != NULL) {
        *capacity = allocator->buffer_size;
    }

    if (handles_used != NULL) {
        *handles_used = allocator->handle_count;
    }

    if (handles_capacity != NULL) {
        // Theoretical maximum handles that could fit if buffer was empty
        *handles_capacity = allocator->buffer_size / sizeof(defrag_handle_entry_t);
    }

    UNLOCK(allocator);
}
