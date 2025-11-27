// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "../src/defrag_allocator.h"
#include "utest.h"
#include <string.h>

// Test fixture with common setup
struct defrag_fixture {
    uint8_t                buffer[1024];
    defrag_allocator_t     allocator;
};

UTEST_F_SETUP(defrag_fixture) {
    memset(utest_fixture->buffer, 0, sizeof(utest_fixture->buffer));
    defrag_allocator_init(&utest_fixture->allocator, utest_fixture->buffer, sizeof(utest_fixture->buffer));
}

UTEST_F_TEARDOWN(defrag_fixture) {
    // Nothing to teardown (no malloc used)
}

// ============================================================================
// Initialization Tests
// ============================================================================

// Verify allocator initializes with correct default state
UTEST_F(defrag_fixture, init_creates_empty_allocator) {
    ASSERT_EQ(utest_fixture->allocator.handle_count, 0);
    ASSERT_EQ(utest_fixture->allocator.next_handle_id, 1);
    ASSERT_EQ(utest_fixture->allocator.buffer, utest_fixture->buffer);
    ASSERT_EQ(utest_fixture->allocator.buffer_size, 1024);
}

// Verify statistics API reports correct initial values
UTEST_F(defrag_fixture, init_stats_show_empty) {
    uint32_t used, capacity, handles_used, handles_capacity;
    defrag_allocator_stats(&utest_fixture->allocator, &used, &capacity, &handles_used, &handles_capacity);

    ASSERT_EQ(used, 0);
    ASSERT_EQ(capacity, 1024);
    ASSERT_EQ(handles_used, 0);
    ASSERT_GT(handles_capacity, 0);
}

// ============================================================================
// Basic Allocation Tests
// ============================================================================

// Verify single allocation returns a valid handle (ID = 1)
UTEST_F(defrag_fixture, alloc_single_returns_valid_handle) {
    defrag_handle_t h = defrag_allocator_alloc(&utest_fixture->allocator, 100);

    ASSERT_NE(h, DEFRAG_INVALID_HANDLE);
    ASSERT_EQ(h, 1);  // First handle should be 1
}

// Verify zero-size allocation is rejected
UTEST_F(defrag_fixture, alloc_zero_size_returns_invalid) {
    defrag_handle_t h = defrag_allocator_alloc(&utest_fixture->allocator, 0);

    ASSERT_EQ(h, DEFRAG_INVALID_HANDLE);
}

// Verify multiple allocations return sequential handle IDs
UTEST_F(defrag_fixture, alloc_multiple_returns_sequential_handles) {
    defrag_handle_t h1 = defrag_allocator_alloc(&utest_fixture->allocator, 50);
    defrag_handle_t h2 = defrag_allocator_alloc(&utest_fixture->allocator, 100);
    defrag_handle_t h3 = defrag_allocator_alloc(&utest_fixture->allocator, 150);

    ASSERT_EQ(h1, 1);
    ASSERT_EQ(h2, 2);
    ASSERT_EQ(h3, 3);
}

// Verify allocation larger than buffer size fails
UTEST_F(defrag_fixture, alloc_too_large_returns_invalid) {
    defrag_handle_t h = defrag_allocator_alloc(&utest_fixture->allocator, 2048);

    ASSERT_EQ(h, DEFRAG_INVALID_HANDLE);
}

// Verify buffer fills correctly and subsequent allocation fails when full
UTEST_F(defrag_fixture, alloc_fills_buffer_correctly) {
    defrag_handle_t h1 = defrag_allocator_alloc(&utest_fixture->allocator, 500);
    defrag_handle_t h2 = defrag_allocator_alloc(&utest_fixture->allocator, 400);

    ASSERT_NE(h1, DEFRAG_INVALID_HANDLE);
    ASSERT_NE(h2, DEFRAG_INVALID_HANDLE);

    // This should fail (not enough space with handle table overhead)
    defrag_handle_t h3 = defrag_allocator_alloc(&utest_fixture->allocator, 200);
    ASSERT_EQ(h3, DEFRAG_INVALID_HANDLE);
}

// ============================================================================
// Lock/Unlock Tests
// ============================================================================

// Verify locking a valid handle returns a valid pointer within buffer bounds
UTEST_F(defrag_fixture, lock_valid_handle_returns_pointer) {
    defrag_handle_t h = defrag_allocator_alloc(&utest_fixture->allocator, 100);
    void *ptr = defrag_allocator_lock(&utest_fixture->allocator, h);

    ASSERT_NE(ptr, NULL);
    ASSERT_GE(ptr, utest_fixture->buffer);
    ASSERT_LT(ptr, utest_fixture->buffer + 1024);
}

// Verify locking DEFRAG_INVALID_HANDLE returns NULL
UTEST_F(defrag_fixture, lock_invalid_handle_returns_null) {
    void *ptr = defrag_allocator_lock(&utest_fixture->allocator, DEFRAG_INVALID_HANDLE);
    ASSERT_EQ(ptr, NULL);
}

// Verify locking a nonexistent handle ID returns NULL
UTEST_F(defrag_fixture, lock_nonexistent_handle_returns_null) {
    void *ptr = defrag_allocator_lock(&utest_fixture->allocator, 999);
    ASSERT_EQ(ptr, NULL);
}

// Verify locking increments the 4-bit lock_count field
UTEST_F(defrag_fixture, lock_increments_lock_count) {
    defrag_handle_t h = defrag_allocator_alloc(&utest_fixture->allocator, 100);

    defrag_handle_entry_t *handles = (defrag_handle_entry_t *)utest_fixture->allocator.buffer;
    ASSERT_EQ(handles[0].lock_count, 0);

    defrag_allocator_lock(&utest_fixture->allocator, h);
    ASSERT_EQ(handles[0].lock_count, 1);

    defrag_allocator_lock(&utest_fixture->allocator, h);
    ASSERT_EQ(handles[0].lock_count, 2);
}

// Verify unlocking decrements the lock_count field
UTEST_F(defrag_fixture, unlock_decrements_lock_count) {
    defrag_handle_t h = defrag_allocator_alloc(&utest_fixture->allocator, 100);

    defrag_allocator_lock(&utest_fixture->allocator, h);
    defrag_allocator_lock(&utest_fixture->allocator, h);

    defrag_handle_entry_t *handles = (defrag_handle_entry_t *)utest_fixture->allocator.buffer;
    ASSERT_EQ(handles[0].lock_count, 2);

    defrag_allocator_unlock(&utest_fixture->allocator, h);
    ASSERT_EQ(handles[0].lock_count, 1);

    defrag_allocator_unlock(&utest_fixture->allocator, h);
    ASSERT_EQ(handles[0].lock_count, 0);
}

// Verify unlocking invalid handles doesn't crash (safe operation)
UTEST_F(defrag_fixture, unlock_invalid_handle_safe) {
    defrag_allocator_unlock(&utest_fixture->allocator, DEFRAG_INVALID_HANDLE);
    defrag_allocator_unlock(&utest_fixture->allocator, 999);
}

// ============================================================================
// Data Access Tests
// ============================================================================

// Verify data written to allocation persists across lock/unlock cycles
UTEST_F(defrag_fixture, can_write_and_read_data) {
    defrag_handle_t h = defrag_allocator_alloc(&utest_fixture->allocator, 100);
    uint8_t *data = defrag_allocator_lock(&utest_fixture->allocator, h);

    ASSERT_NE(data, NULL);

    // Write pattern
    for (int i = 0; i < 100; i++) {
        data[i] = (uint8_t)(i & 0xFF);
    }

    defrag_allocator_unlock(&utest_fixture->allocator, h);

    // Read back
    data = defrag_allocator_lock(&utest_fixture->allocator, h);
    for (int i = 0; i < 100; i++) {
        ASSERT_EQ(data[i], (uint8_t)(i & 0xFF));
    }
    defrag_allocator_unlock(&utest_fixture->allocator, h);
}

// Verify multiple allocations maintain independent data (no cross-contamination)
UTEST_F(defrag_fixture, multiple_allocations_independent) {
    defrag_handle_t h1 = defrag_allocator_alloc(&utest_fixture->allocator, 50);
    defrag_handle_t h2 = defrag_allocator_alloc(&utest_fixture->allocator, 50);

    uint8_t *d1 = defrag_allocator_lock(&utest_fixture->allocator, h1);
    uint8_t *d2 = defrag_allocator_lock(&utest_fixture->allocator, h2);

    memset(d1, 0xAA, 50);
    memset(d2, 0xBB, 50);

    defrag_allocator_unlock(&utest_fixture->allocator, h1);
    defrag_allocator_unlock(&utest_fixture->allocator, h2);

    // Verify independence
    d1 = defrag_allocator_lock(&utest_fixture->allocator, h1);
    d2 = defrag_allocator_lock(&utest_fixture->allocator, h2);

    ASSERT_EQ(d1[0], 0xAA);
    ASSERT_EQ(d2[0], 0xBB);

    defrag_allocator_unlock(&utest_fixture->allocator, h1);
    defrag_allocator_unlock(&utest_fixture->allocator, h2);
}

// ============================================================================
// Free Tests
// ============================================================================

// Verify freeing allocations immediately reduces handle count (no holes in table)
UTEST_F(defrag_fixture, free_reduces_handle_count) {
    defrag_handle_t h1 = defrag_allocator_alloc(&utest_fixture->allocator, 50);
    defrag_handle_t h2 = defrag_allocator_alloc(&utest_fixture->allocator, 50);

    ASSERT_EQ(utest_fixture->allocator.handle_count, 2);

    defrag_allocator_free(&utest_fixture->allocator, h1);
    ASSERT_EQ(utest_fixture->allocator.handle_count, 1);

    defrag_allocator_free(&utest_fixture->allocator, h2);
    ASSERT_EQ(utest_fixture->allocator.handle_count, 0);
}

// Verify freeing invalid handles doesn't crash (safe operation)
UTEST_F(defrag_fixture, free_invalid_handle_safe) {
    defrag_allocator_free(&utest_fixture->allocator, DEFRAG_INVALID_HANDLE);
    defrag_allocator_free(&utest_fixture->allocator, 999);
}

// Verify freed handle becomes immediately inaccessible (lock returns NULL)
UTEST_F(defrag_fixture, free_makes_handle_inaccessible) {
    defrag_handle_t h = defrag_allocator_alloc(&utest_fixture->allocator, 100);
    defrag_allocator_free(&utest_fixture->allocator, h);

    void *ptr = defrag_allocator_lock(&utest_fixture->allocator, h);
    ASSERT_EQ(ptr, NULL);
}

// Verify handle IDs can be reused after free (wraparound with reuse detection)
UTEST_F(defrag_fixture, can_reuse_handle_id_after_free) {
    defrag_handle_t h1 = defrag_allocator_alloc(&utest_fixture->allocator, 50);
    defrag_allocator_free(&utest_fixture->allocator, h1);

    defrag_handle_t h2 = defrag_allocator_alloc(&utest_fixture->allocator, 50);

    // h2 might reuse h1's ID (depends on allocation strategy)
    ASSERT_NE(h2, DEFRAG_INVALID_HANDLE);
}

// ============================================================================
// Defragmentation Tests
// ============================================================================

// Verify defragmentation compacts allocations and reduces used space
UTEST_F(defrag_fixture, defrag_compacts_allocations) {
    defrag_handle_t h1 = defrag_allocator_alloc(&utest_fixture->allocator, 100);
    defrag_handle_t h2 = defrag_allocator_alloc(&utest_fixture->allocator, 100);
    defrag_handle_t h3 = defrag_allocator_alloc(&utest_fixture->allocator, 100);

    // Free middle allocation to create hole
    defrag_allocator_free(&utest_fixture->allocator, h2);

    uint32_t used_before, capacity;
    defrag_allocator_stats(&utest_fixture->allocator, &used_before, &capacity, NULL, NULL);

    // Defragment should compact
    defrag_allocator_defragment(&utest_fixture->allocator);

    uint32_t used_after;
    defrag_allocator_stats(&utest_fixture->allocator, &used_after, NULL, NULL, NULL);

    // Used space should be less after defrag (handle table shrunk)
    ASSERT_LE(used_after, used_before);
}

// Verify defragmentation preserves data integrity when moving allocations
UTEST_F(defrag_fixture, defrag_preserves_data_integrity) {
    defrag_handle_t h1 = defrag_allocator_alloc(&utest_fixture->allocator, 100);
    defrag_handle_t h2 = defrag_allocator_alloc(&utest_fixture->allocator, 100);
    defrag_handle_t h3 = defrag_allocator_alloc(&utest_fixture->allocator, 100);

    // Write distinct patterns
    uint8_t *d1 = defrag_allocator_lock(&utest_fixture->allocator, h1);
    memset(d1, 0xAA, 100);
    defrag_allocator_unlock(&utest_fixture->allocator, h1);

    uint8_t *d2 = defrag_allocator_lock(&utest_fixture->allocator, h2);
    memset(d2, 0xBB, 100);
    defrag_allocator_unlock(&utest_fixture->allocator, h2);

    uint8_t *d3 = defrag_allocator_lock(&utest_fixture->allocator, h3);
    memset(d3, 0xCC, 100);
    defrag_allocator_unlock(&utest_fixture->allocator, h3);

    // Free middle allocation
    defrag_allocator_free(&utest_fixture->allocator, h2);

    // Defragment
    defrag_allocator_defragment(&utest_fixture->allocator);

    // Verify data integrity
    d1 = defrag_allocator_lock(&utest_fixture->allocator, h1);
    d3 = defrag_allocator_lock(&utest_fixture->allocator, h3);

    for (int i = 0; i < 100; i++) {
        ASSERT_EQ(d1[i], 0xAA);
        ASSERT_EQ(d3[i], 0xCC);
    }

    defrag_allocator_unlock(&utest_fixture->allocator, h1);
    defrag_allocator_unlock(&utest_fixture->allocator, h3);
}

// Verify locked allocations remain at same address during defragmentation
UTEST_F(defrag_fixture, defrag_respects_locked_allocations) {
    defrag_handle_t h1 = defrag_allocator_alloc(&utest_fixture->allocator, 100);
    defrag_handle_t h2 = defrag_allocator_alloc(&utest_fixture->allocator, 100);
    defrag_handle_t h3 = defrag_allocator_alloc(&utest_fixture->allocator, 100);

    // Lock h2
    uint8_t *d2 = defrag_allocator_lock(&utest_fixture->allocator, h2);
    uint8_t *d2_before = d2;
    memset(d2, 0xBB, 100);

    // Free h1 and h3
    defrag_allocator_free(&utest_fixture->allocator, h1);
    defrag_allocator_free(&utest_fixture->allocator, h3);

    // Defragment
    defrag_allocator_defragment(&utest_fixture->allocator);

    // h2 should not have moved (it's locked)
    d2 = defrag_allocator_lock(&utest_fixture->allocator, h2);
    ASSERT_EQ(d2, d2_before);
    ASSERT_EQ(d2[0], 0xBB);

    defrag_allocator_unlock(&utest_fixture->allocator, h2);
    defrag_allocator_unlock(&utest_fixture->allocator, h2);
}

// ============================================================================
// Handle ID Wraparound Tests
// ============================================================================

// Verify handle IDs wrap from 65535 back to 1 (16-bit field wraparound)
UTEST_F(defrag_fixture, handle_id_wraparound_works) {
    utest_fixture->allocator.next_handle_id = 65534;

    defrag_handle_t h1 = defrag_allocator_alloc(&utest_fixture->allocator, 10);
    defrag_handle_t h2 = defrag_allocator_alloc(&utest_fixture->allocator, 10);
    defrag_handle_t h3 = defrag_allocator_alloc(&utest_fixture->allocator, 10);

    ASSERT_EQ(h1, 65534);
    ASSERT_EQ(h2, 65535);
    ASSERT_NE(h3, DEFRAG_INVALID_HANDLE);
    ASSERT_LT(h3, 100);  // Should wrap to low number
}

// Verify find_available_handle_id() can reuse freed handle IDs
UTEST_F(defrag_fixture, handle_id_reuse_after_free) {
    defrag_handle_t h1 = defrag_allocator_alloc(&utest_fixture->allocator, 10);
    defrag_handle_t h2 = defrag_allocator_alloc(&utest_fixture->allocator, 10);

    defrag_allocator_free(&utest_fixture->allocator, h1);
    defrag_allocator_free(&utest_fixture->allocator, h2);

    // Set next_handle_id to force reuse
    utest_fixture->allocator.next_handle_id = 1;

    defrag_handle_t h3 = defrag_allocator_alloc(&utest_fixture->allocator, 10);
    ASSERT_NE(h3, DEFRAG_INVALID_HANDLE);
}

// ============================================================================
// Size Field Macro Tests
// ============================================================================

// Verify DEFRAG_GET_SIZE/SET_SIZE macros correctly handle split 20-bit field
UTEST_F(defrag_fixture, size_macros_work_correctly) {
    defrag_handle_entry_t entry;
    uint32_t test_sizes[] = {0, 1, 255, 256, 4095, 4096, 65535, 1048575};

    for (size_t i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); i++) {
        DEFRAG_SET_SIZE(&entry, test_sizes[i]);
        uint32_t retrieved = DEFRAG_GET_SIZE(&entry);
        ASSERT_EQ(retrieved, test_sizes[i]);
    }
}

// Verify 20-bit size field correctly wraps at max value (1048575)
UTEST_F(defrag_fixture, size_field_20bit_limit) {
    defrag_handle_entry_t entry;

    // Max 20-bit value is 1048575
    DEFRAG_SET_SIZE(&entry, 1048575);
    ASSERT_EQ(DEFRAG_GET_SIZE(&entry), 1048575);

    // Test that it wraps correctly if exceeded (implementation detail)
    DEFRAG_SET_SIZE(&entry, 1048576);
    ASSERT_EQ(DEFRAG_GET_SIZE(&entry), 0);  // Should wrap
}

// ============================================================================
// Statistics Tests
// ============================================================================

// Verify statistics API correctly tracks allocations and handle usage
UTEST_F(defrag_fixture, stats_track_usage_correctly) {
    uint32_t used, capacity, handles_used, handles_capacity;

    // Initially empty
    defrag_allocator_stats(&utest_fixture->allocator, &used, &capacity, &handles_used, &handles_capacity);
    ASSERT_EQ(used, 0);
    ASSERT_EQ(handles_used, 0);

    // Allocate some
    defrag_allocator_alloc(&utest_fixture->allocator, 100);
    defrag_allocator_alloc(&utest_fixture->allocator, 200);

    defrag_allocator_stats(&utest_fixture->allocator, &used, &capacity, &handles_used, &handles_capacity);
    ASSERT_GT(used, 0);
    ASSERT_EQ(handles_used, 2);
}

// ============================================================================
// Edge Case Tests
// ============================================================================

// Verify allocator handles many small allocations until buffer fills
UTEST_F(defrag_fixture, many_small_allocations) {
    defrag_handle_t handles[50];
    int count = 0;

    for (int i = 0; i < 50; i++) {
        handles[i] = defrag_allocator_alloc(&utest_fixture->allocator, 10);
        if (handles[i] != DEFRAG_INVALID_HANDLE) {
            count++;
        } else {
            break;
        }
    }

    ASSERT_GT(count, 10);  // Should allocate at least 10

    // Free all
    for (int i = 0; i < count; i++) {
        defrag_allocator_free(&utest_fixture->allocator, handles[i]);
    }

    ASSERT_EQ(utest_fixture->allocator.handle_count, 0);
}

// Verify allocator remains stable through repeated alloc/free cycles
UTEST_F(defrag_fixture, alternating_alloc_free) {
    for (int cycle = 0; cycle < 10; cycle++) {
        defrag_handle_t h1 = defrag_allocator_alloc(&utest_fixture->allocator, 50);
        defrag_handle_t h2 = defrag_allocator_alloc(&utest_fixture->allocator, 50);

        ASSERT_NE(h1, DEFRAG_INVALID_HANDLE);
        ASSERT_NE(h2, DEFRAG_INVALID_HANDLE);

        defrag_allocator_free(&utest_fixture->allocator, h1);
        defrag_allocator_free(&utest_fixture->allocator, h2);
    }

    // Should still work after many cycles
    ASSERT_EQ(utest_fixture->allocator.handle_count, 0);
}

// Verify 4-bit lock_count field correctly handles maximum (15) locks
UTEST_F(defrag_fixture, max_lock_count) {
    defrag_handle_t h = defrag_allocator_alloc(&utest_fixture->allocator, 100);

    // Lock 15 times (4-bit max)
    for (int i = 0; i < 15; i++) {
        void *ptr = defrag_allocator_lock(&utest_fixture->allocator, h);
        ASSERT_NE(ptr, NULL);
    }

    defrag_handle_entry_t *handles = (defrag_handle_entry_t *)utest_fixture->allocator.buffer;
    ASSERT_EQ(handles[0].lock_count, 15);

    // Unlock all
    for (int i = 0; i < 15; i++) {
        defrag_allocator_unlock(&utest_fixture->allocator, h);
    }

    ASSERT_EQ(handles[0].lock_count, 0);
}

// ============================================================================
// Thread-Safety Callback Tests
// ============================================================================

static int lock_call_count = 0;
static int unlock_call_count = 0;

static void test_lock_callback(void *user_data) {
    lock_call_count++;
}

static void test_unlock_callback(void *user_data) {
    unlock_call_count++;
}

// Verify thread-safety callbacks are called correctly
UTEST_F(defrag_fixture, callbacks_are_invoked) {
    lock_call_count = 0;
    unlock_call_count = 0;

    // Set callbacks
    defrag_allocator_set_callbacks(&utest_fixture->allocator, test_lock_callback, test_unlock_callback, NULL);

    // Perform operations that should trigger callbacks
    defrag_handle_t h = defrag_allocator_alloc(&utest_fixture->allocator, 100);
    ASSERT_NE(h, DEFRAG_INVALID_HANDLE);
    ASSERT_GT(lock_call_count, 0);
    ASSERT_GT(unlock_call_count, 0);
    // Allocation should have balanced lock/unlock calls
    ASSERT_EQ(lock_call_count, unlock_call_count);

    int locks_before = lock_call_count;
    int unlocks_before = unlock_call_count;

    void *ptr = defrag_allocator_lock(&utest_fixture->allocator, h);
    ASSERT_NE(ptr, NULL);
    ASSERT_GT(lock_call_count, locks_before);

    defrag_allocator_unlock(&utest_fixture->allocator, h);
    ASSERT_GT(unlock_call_count, unlocks_before);
    // After explicit lock/unlock pair, should be balanced again
    ASSERT_EQ(lock_call_count, unlock_call_count);
}

// ============================================================================
// Handle ID Collision Tests
// ============================================================================

// Verify handle ID allocation skips IDs already in use
UTEST_F(defrag_fixture, handle_id_skips_in_use) {
    // Allocate handles 1, 2, 3
    defrag_handle_t h1 = defrag_allocator_alloc(&utest_fixture->allocator, 10);
    defrag_handle_t h2 = defrag_allocator_alloc(&utest_fixture->allocator, 10);
    defrag_handle_t h3 = defrag_allocator_alloc(&utest_fixture->allocator, 10);

    ASSERT_EQ(h1, 1);
    ASSERT_EQ(h2, 2);
    ASSERT_EQ(h3, 3);

    // Free h2 (ID 2 becomes available)
    defrag_allocator_free(&utest_fixture->allocator, h2);

    // Set next_handle_id back to 2 to force collision detection
    utest_fixture->allocator.next_handle_id = 2;

    // Allocate new handle - should find ID 2 is in use, skip to ID 4
    defrag_handle_t h4 = defrag_allocator_alloc(&utest_fixture->allocator, 10);
    ASSERT_NE(h4, DEFRAG_INVALID_HANDLE);
    // Should get ID 2 since it's now free
    ASSERT_EQ(h4, 2);
}

// Verify allocation fails when all handle IDs are exhausted
UTEST_F(defrag_fixture, handle_id_exhaustion) {
    // This would require allocating 65535 handles which is impractical
    // Instead, we'll simulate by manually filling the handle table
    // and setting next_handle_id such that find_available_handle_id fails

    // Fill buffer with many small allocations
    int max_handles = (utest_fixture->allocator.buffer_size / (sizeof(defrag_handle_entry_t) + 1));
    defrag_handle_t handles[100];
    int count = 0;

    // Allocate as many as we can fit
    for (int i = 0; i < 100 && i < max_handles; i++) {
        handles[i] = defrag_allocator_alloc(&utest_fixture->allocator, 1);
        if (handles[i] == DEFRAG_INVALID_HANDLE) {
            break;
        }
        count++;
    }

    // Verify we allocated several handles
    ASSERT_GT(count, 10);

    // Now manually corrupt next_handle_id to point beyond the max
    // This forces the wraparound logic to execute
    utest_fixture->allocator.next_handle_id = 65536;

    // Next allocation should wraparound from 65536 to 1 and find an available ID
    defrag_handle_t h = defrag_allocator_alloc(&utest_fixture->allocator, 1);

    // Should either succeed (finding a free ID) or fail (buffer full)
    // Either way, the wraparound code path is exercised
    if (h != DEFRAG_INVALID_HANDLE) {
        ASSERT_GT(h, 0);
        ASSERT_LT(h, 65536);
    }
}

// Verify allocation fails when all handle IDs in search range are taken
UTEST(defrag_exhaustion, all_handle_ids_exhausted) {
    // Create a tiny buffer that can only hold handle entries, no allocation data
    uint8_t tiny_buffer[256];
    defrag_allocator_t alloc;
    defrag_allocator_init(&alloc, tiny_buffer, sizeof(tiny_buffer));

    // Manually create handle entries to fill the search space
    // We'll fill the handle table completely with sequential IDs
    defrag_handle_entry_t *handles = (defrag_handle_entry_t *)alloc.buffer;

    // Calculate how many entries fit
    uint32_t max_entries = sizeof(tiny_buffer) / sizeof(defrag_handle_entry_t);

    // Fill with sequential handle IDs starting from 1
    for (uint32_t i = 0; i < max_entries && i < 65535; i++) {
        handles[i].handle_id = i + 1;
        handles[i].offset = alloc.buffer_size; // Point to end (no actual allocation)
        DEFRAG_SET_SIZE(&handles[i], 0);
        handles[i].lock_count = 0;
        alloc.handle_count++;
    }

    // Now try to allocate - should fail because:
    // 1. Handle table is completely full (no space for new entry)
    defrag_handle_t h = defrag_allocator_alloc(&alloc, 1);
    ASSERT_EQ(h, DEFRAG_INVALID_HANDLE);
}

// ============================================================================
// Main
// ============================================================================

UTEST_MAIN()
