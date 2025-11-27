// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Handle type - 0 is invalid
typedef uint32_t defrag_handle_t;
#define DEFRAG_INVALID_HANDLE 0

// Ultra-compact handle entry (8 bytes instead of 16 - 50% space savings!)
// Optimized for embedded systems where every byte counts
//
// Constraints (realistic for most embedded systems):
//   - Buffer size: 16MB max (24-bit offset)
//   - Per-allocation: 1MB max (20-bit size)
//   - Handle IDs: 65,535 max (16-bit ID)
//   - Locks: 15 max (4-bit count)
//
// Note: Size field split across storage units for alignment safety
typedef struct {
    // First 32-bit unit
    uint32_t handle_id : 16; // Handle ID (stable identifier, 65K max)
    uint32_t lock_count : 4; // Lock count (15 max simultaneous locks)
    uint32_t size_high : 12; // Upper 12 bits of 20-bit size

    // Second 32-bit unit
    uint32_t size_low : 8; // Lower 8 bits of size (total 20 bits = 1MB max)
    uint32_t offset : 24;  // Offset from buffer start (16MB max)
} defrag_handle_entry_t;

// Helper macros to access 20-bit size field
#define DEFRAG_GET_SIZE(entry) (((uint32_t)(entry)->size_low) | (((uint32_t)(entry)->size_high) << 8))
#define DEFRAG_SET_SIZE(entry, val)                \
    do {                                           \
        (entry)->size_low  = (val) & 0xFF;         \
        (entry)->size_high = ((val) >> 8) & 0xFFF; \
    } while (0)

// Compile-time size check
_Static_assert(sizeof(defrag_handle_entry_t) == 8, "Handle entry should be 8 bytes");

/*
 * EVEN MORE COMPACT ALTERNATIVE (if you need tighter memory budgets):
 *
 * For ultra-constrained systems, you could further reduce to:
 *   - 12-bit handle_id (4,095 handles)
 *   - 4-bit lock_count (15 locks)
 *   - 20-bit offset (1MB buffer)
 *   - 12-bit size (4KB per allocation)
 *   Total: 48 bits = 6 bytes (with 16-bit padding = 8 bytes aligned)
 *
 * Trade-off: Smaller buffer/allocations for slightly more handles per KB
 */

// Allocator structure
typedef struct {
    uint8_t *buffer;      // Fixed-size memory buffer
    uint32_t buffer_size; // Total buffer size

    uint32_t handle_count;   // Current number of handle entries
    uint32_t next_handle_id; // Next handle ID to allocate (monotonically increasing)

    // Thread-safety callbacks (optional)
    void (*lock_fn)(void *user_data);
    void (*unlock_fn)(void *user_data);
    void *user_data;
} defrag_allocator_t;

// Initialize allocator with a fixed buffer
// Handle table will be stored at the beginning of the buffer and grows as needed
// buffer: pre-allocated memory buffer
// buffer_size: size of the buffer
void defrag_allocator_init(defrag_allocator_t *allocator, void *buffer, uint32_t buffer_size);

// Set optional thread-safety callbacks
void defrag_allocator_set_callbacks(defrag_allocator_t *allocator, void (*lock_fn)(void *), void (*unlock_fn)(void *), void *user_data);

// Allocate memory, returns handle (DEFRAG_INVALID_HANDLE on failure)
defrag_handle_t defrag_allocator_alloc(defrag_allocator_t *allocator, uint32_t size);

// Lock a handle and get pointer to its memory (NULL if invalid handle)
// Must be unlocked before defragmentation can move it
void *defrag_allocator_lock(defrag_allocator_t *allocator, defrag_handle_t handle);

// Unlock a handle (allows it to be moved during defragmentation)
void defrag_allocator_unlock(defrag_allocator_t *allocator, defrag_handle_t handle);

// Free an allocation
// Note: Handle must be unlocked (lock_count == 0)
void defrag_allocator_free(defrag_allocator_t *allocator, defrag_handle_t handle);

// Manually trigger defragmentation (compacts memory)
// Only unlocked allocations will be moved
void defrag_allocator_defragment(defrag_allocator_t *allocator);

// Get allocator statistics
void defrag_allocator_stats(defrag_allocator_t *allocator, uint32_t *used, uint32_t *capacity, uint32_t *handles_used, uint32_t *handles_capacity);

#ifdef __cplusplus
}
#endif
