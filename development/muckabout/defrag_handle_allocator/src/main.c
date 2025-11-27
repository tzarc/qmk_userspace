// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "defrag_allocator.h"

// Example: demonstrating the defragmenting handle allocator

void print_stats(defrag_allocator_t *alloc, const char *label) {
    uint32_t used, capacity, handles_used, handles_capacity;
    defrag_allocator_stats(alloc, &used, &capacity, &handles_used, &handles_capacity);
    printf("[%s] Memory: %u/%u bytes (%.1f%%), Handles: %u/%u (table size: %u bytes)\n", label, used, capacity, (used * 100.0f) / capacity, handles_used, handles_capacity, handles_used * (uint32_t)sizeof(defrag_handle_entry_t));
}

void print_allocation_map(defrag_allocator_t *alloc) {
    defrag_handle_entry_t *handles = (defrag_handle_entry_t *)alloc->buffer;
    printf("Allocation map:\n");
    printf("  Handle table: 0-%u (size=%u bytes)\n", alloc->handle_count * (uint32_t)sizeof(defrag_handle_entry_t), alloc->handle_count * (uint32_t)sizeof(defrag_handle_entry_t));
    for (uint32_t i = 0; i < alloc->handle_count; i++) {
        printf("  Handle %u: offset=%u, size=%u, locks=%u\n", handles[i].handle_id, handles[i].offset, DEFRAG_GET_SIZE(&handles[i]), handles[i].lock_count);
    }
}

int main(void) {
    printf("=== Defragmenting Handle Allocator Demo ===\n\n");

    // Setup allocator with a single buffer (handles stored inside!)
    uint8_t            buffer[1024];
    defrag_allocator_t allocator;

    defrag_allocator_init(&allocator, buffer, sizeof(buffer));
    print_stats(&allocator, "Initial");
    printf("\n");

    // Test 1: Basic allocation
    printf("Test 1: Basic allocations\n");
    defrag_handle_t h1 = defrag_allocator_alloc(&allocator, 100);
    defrag_handle_t h2 = defrag_allocator_alloc(&allocator, 200);
    defrag_handle_t h3 = defrag_allocator_alloc(&allocator, 150);
    printf("Allocated h1=%u (100 bytes), h2=%u (200 bytes), h3=%u (150 bytes)\n", h1, h2, h3);
    print_stats(&allocator, "After alloc");
    print_allocation_map(&allocator);
    printf("\n");

    // Test 2: Lock and write data
    printf("Test 2: Lock, write data, unlock\n");
    uint8_t *data = defrag_allocator_lock(&allocator, h1);
    if (data) {
        memset(data, 0xAA, 100);
        printf("Wrote pattern 0xAA to h1\n");
        defrag_allocator_unlock(&allocator, h1);
    }
    data = defrag_allocator_lock(&allocator, h2);
    if (data) {
        memset(data, 0xBB, 200);
        printf("Wrote pattern 0xBB to h2\n");
        defrag_allocator_unlock(&allocator, h2);
    }
    printf("\n");

    // Test 3: Free middle allocation (creates fragmentation)
    printf("Test 3: Free middle allocation h2\n");
    defrag_allocator_free(&allocator, h2);
    printf("Freed h2 - creates hole in memory\n");
    print_stats(&allocator, "After free");
    print_allocation_map(&allocator);
    printf("\n");

    // Test 4: Allocate more (will be placed at end, not in the hole)
    printf("Test 4: Allocate after freeing\n");
    defrag_handle_t h4 = defrag_allocator_alloc(&allocator, 50);
    defrag_handle_t h5 = defrag_allocator_alloc(&allocator, 75);
    printf("Allocated h4=%u (50 bytes), h5=%u (75 bytes)\n", h4, h5);
    print_stats(&allocator, "After more allocs");
    print_allocation_map(&allocator);
    printf("\n");

    // Test 5: Defragment to reclaim fragmented space
    printf("Test 5: Defragment memory\n");
    defrag_allocator_defragment(&allocator);
    printf("Defragmented - allocations compacted, handle table shrunk\n");
    print_stats(&allocator, "After defrag");
    print_allocation_map(&allocator);
    printf("\n");

    // Test 6: Verify data integrity after defragmentation
    printf("Test 6: Verify data integrity\n");
    data = defrag_allocator_lock(&allocator, h1);
    if (data) {
        int ok = 1;
        for (int i = 0; i < 100; i++) {
            if (data[i] != 0xAA) {
                ok = 0;
                break;
            }
        }
        printf("h1 data integrity: %s\n", ok ? "OK" : "CORRUPTED");
        defrag_allocator_unlock(&allocator, h1);
    }
    printf("\n");

    // Test 7: Defragmentation with locked allocations
    printf("Test 7: Defragment with locked allocation\n");
    defrag_handle_t h6 = defrag_allocator_alloc(&allocator, 60);
    data               = defrag_allocator_lock(&allocator, h3); // Lock h3
    printf("Allocated h6=%u, locked h3\n", h6);
    print_allocation_map(&allocator);

    defrag_allocator_free(&allocator, h4); // Free h4 to create gap
    printf("Freed h4, attempting defrag with h3 locked...\n");
    defrag_allocator_defragment(&allocator);

    printf("After defrag (h3 should not move, others compacted):\n");
    print_allocation_map(&allocator);

    defrag_allocator_unlock(&allocator, h3);
    printf("\n");

    // Test 8: Test allocation failure when full
    printf("Test 8: Allocation when nearly full\n");
    defrag_handle_t h7 = defrag_allocator_alloc(&allocator, 800);
    if (h7 == DEFRAG_INVALID_HANDLE) {
        printf("Large allocation failed (expected - insufficient space)\n");
    } else {
        printf("Allocated h7=%u (800 bytes)\n", h7);
    }
    print_stats(&allocator, "Final");
    printf("\n");

    // Test 9: Test invalid handle operations
    printf("Test 9: Invalid handle operations\n");
    void *invalid_ptr = defrag_allocator_lock(&allocator, DEFRAG_INVALID_HANDLE);
    printf("Lock invalid handle: %s\n", invalid_ptr == NULL ? "NULL (correct)" : "ERROR");

    invalid_ptr = defrag_allocator_lock(&allocator, 999);
    printf("Lock non-existent handle: %s\n", invalid_ptr == NULL ? "NULL (correct)" : "ERROR");
    printf("\n");

    // Test 10: Test handle table growth
    printf("Test 10: Allocate many small items to test handle table growth\n");
    defrag_handle_t handles[20];
    int             alloc_count = 0;
    for (int i = 0; i < 20; i++) {
        handles[i] = defrag_allocator_alloc(&allocator, 10);
        if (handles[i] != DEFRAG_INVALID_HANDLE) {
            alloc_count++;
        } else {
            printf("Allocation %d failed (buffer full)\n", i + 1);
            break;
        }
    }
    printf("Allocated %d small items\n", alloc_count);
    print_stats(&allocator, "After many allocs");
    printf("\n");

    // Free them all and defrag to shrink handle table
    printf("Test 11: Free all small items and defragment\n");
    for (int i = 0; i < alloc_count; i++) {
        defrag_allocator_free(&allocator, handles[i]);
    }
    printf("Freed %d items\n", alloc_count);
    print_stats(&allocator, "After freeing");

    defrag_allocator_defragment(&allocator);
    printf("After defrag - handle table should shrink\n");
    print_stats(&allocator, "After defrag");
    printf("\n");

    // Test 12: Handle ID wraparound protection
    printf("Test 12: Handle ID wraparound\n");
    // Simulate wraparound by setting next_handle_id near the limit
    allocator.next_handle_id = 65534;

    defrag_handle_t h_wrap1 = defrag_allocator_alloc(&allocator, 10);
    defrag_handle_t h_wrap2 = defrag_allocator_alloc(&allocator, 10);
    defrag_handle_t h_wrap3 = defrag_allocator_alloc(&allocator, 10);

    printf("Allocated near wraparound: h=%u, h=%u, h=%u\n", h_wrap1, h_wrap2, h_wrap3);

    // Free them
    defrag_allocator_free(&allocator, h_wrap1);
    defrag_allocator_free(&allocator, h_wrap2);
    defrag_allocator_free(&allocator, h_wrap3);

    // Verify we can allocate more after wraparound
    defrag_handle_t h_after = defrag_allocator_alloc(&allocator, 10);
    printf("After wraparound, reused ID: h=%u (should be low number)\n", h_after);

    if (h_after != DEFRAG_INVALID_HANDLE && h_after < 100) {
        printf("✓ Wraparound and ID reuse working correctly\n");
    } else {
        printf("✗ Wraparound issue detected\n");
    }
    printf("\n");

    printf("=== All tests complete ===\n");
    return 0;
}
