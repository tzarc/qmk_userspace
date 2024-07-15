/**
 * @file bumparena.h
 * @brief This file implements a bump allocator atop an arena.
 *
 * The bumparena_t struct represents a bump arena, which is a memory management technique that allows
 * for efficient allocation and deallocation of memory in a stack-like manner.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Allow for nullptr with compilers targeting standards below C23. */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ <= 201710L
#    define nullptr ((void *)0)
#endif

/**
 * @struct bumparena_t
 * @brief Represents a bump arena.
 *
 * The bumparena_t struct represents a bump arena, which is a memory management technique that allows
 * for efficient allocation and deallocation of memory in a stack-like manner.
 */
typedef struct bumparena_t {
    uint8_t *buffer;   /**< Pointer to the buffer used by the bump arena. */
    size_t   capacity; /**< The capacity of the bump arena. */
    size_t   offset;   /**< The current offset within the bump arena. */
} bumparena_t;

/**
 * @brief Initializes a bump arena with the specified buffer and capacity.
 *
 * This function initializes the bump arena pointed to by `arena` with the provided `buffer` and `capacity`.
 * The `buffer` should be a pointer to a memory region that can hold at least `capacity` bytes.
 * The `capacity` parameter specifies the maximum amount of memory that can be allocated within the bump arena.
 *
 * @param arena Pointer to the bump arena to be initialized.
 * @param buffer Pointer to the buffer used by the bump arena.
 * @param capacity The capacity of the bump arena.
 * @return `true` if the bump arena was successfully initialized, `false` otherwise.
 */
bool bumparena_init(bumparena_t *arena, void *buffer, size_t capacity);

/**
 * @brief Resets the bump arena to its initial state.
 *
 * This function resets the bump arena pointed to by `arena` to its initial state.
 * This means that the offset within the bump arena is set to 0, effectively deallocating all previously allocated memory.
 *
 * @param arena Pointer to the bump arena to be reset.
 */
void bumparena_reset(bumparena_t *arena);

/**
 * @brief Allocates memory from the bump arena.
 *
 * This function allocates a block of memory of the specified `size` from the bump arena pointed to by `arena`.
 * The `size` parameter specifies the number of bytes to allocate.
 * The allocated memory is aligned to the maximum alignment requirement of the platform.
 *
 * @param arena Pointer to the bump arena.
 * @param size The size of the memory block to allocate.
 * @return A pointer to the allocated memory block, or `nullptr` if the allocation failed.
 */
void *bumparena_alloc(bumparena_t *arena, size_t size);
