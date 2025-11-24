// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file future.h
 * @brief Promise/Future pattern implementation for ChibiOS
 *
 * This library implements a type-safe promise/future pattern using ChibiOS
 * synchronization primitives. It provides a way for asynchronous operations
 * to communicate results between producer and consumer threads.
 *
 * Architecture:
 * - future_state_s: Shared state containing synchronization primitives and buffers
 * - promise_t: Write-side handle used by producer to fulfill/reject
 * - future_t: Read-side handle used by consumer to wait and retrieve results
 *
 * Design characteristics:
 * - Zero dynamic allocations (stack-based with pre-allocated buffers)
 * - Thread-safe using ChibiOS mutex and binary semaphore
 * - Type-safe separation between producer (promise) and consumer (future)
 * - Single producer, single consumer pattern
 *
 * Example usage:
 * @code
 *   future_state_s shared;
 *   promise_t promise;
 *   future_t future;
 *   int result_buf;
 *   char error_buf[256];
 *
 *   // Create promise/future pair
 *   promise_new(&shared, &result_buf, sizeof(result_buf),
 *               error_buf, sizeof(error_buf), &promise, &future);
 *
 *   // Producer thread: fulfill the promise
 *   int value = 42;
 *   promise_fulfill(&promise, &value, sizeof(value));
 *
 *   // Consumer thread: wait for result
 *   if (future_wait(&future) == FUTURE_STATE_FULFILLED) {
 *       int result;
 *       future_get_result(&future, &result, sizeof(result));
 *   }
 *
 *   promise_deinit(&shared);
 * @endcode
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "ch.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief State of a future/promise
 */
typedef enum {
    FUTURE_STATE_PENDING,   ///< Operation not yet completed
    FUTURE_STATE_FULFILLED, ///< Operation completed successfully
    FUTURE_STATE_REJECTED,  ///< Operation failed with error
} future_state_t;

/**
 * @brief Shared state between promise and future
 *
 * This structure holds all the synchronization primitives and data buffers
 * shared between a promise and its corresponding future. It should be
 * allocated on the stack by the creator of the promise/future pair.
 *
 * Thread-safety: Access to this structure is protected by the internal mutex.
 * Direct access to fields should not be performed by users.
 */
typedef struct {
    mutex_t            mutex;               ///< Protects state transitions and data access
    binary_semaphore_t signal;              ///< Signals completion to waiting threads
    future_state_t     state;               ///< Current state (pending/fulfilled/rejected)
    void              *result_buffer;       ///< Buffer for successful result data
    void              *error_buffer;        ///< Buffer for error data
    size_t             result_buffer_size;  ///< Maximum size of result buffer
    size_t             error_buffer_size;   ///< Maximum size of error buffer
    size_t             result_size;         ///< Actual size of stored result
    size_t             error_size;          ///< Actual size of stored error
} future_state_s;

/**
 * @brief Future handle (read-side)
 *
 * A future represents the read-side of an asynchronous operation. The consumer
 * thread uses this to wait for completion and retrieve the result or error.
 *
 * Futures are read-only and provide methods to:
 * - Wait for completion (blocking or with timeout)
 * - Query current state (pending/fulfilled/rejected)
 * - Retrieve the result or error value
 */
typedef struct future_t {
    future_state_s *shared;  ///< Pointer to shared state
} future_t;

/**
 * @brief Promise handle (write-side)
 *
 * A promise represents the write-side of an asynchronous operation. The producer
 * thread uses this to fulfill or reject the operation.
 *
 * Promises can only be fulfilled or rejected once. Subsequent attempts will fail.
 */
typedef struct promise_t {
    future_state_s *shared;  ///< Pointer to shared state
} promise_t;

/**
 * @brief Create a new promise/future pair with pre-allocated buffers
 *
 * Initializes the shared state and creates both a promise (write-side) and
 * future (read-side) that reference it. The shared state, buffers, promise,
 * and future should all be stack-allocated by the caller.
 *
 * The result and error buffers are used to store the data passed to
 * promise_fulfill() or promise_reject(). These buffers must remain valid
 * for the lifetime of the promise/future pair.
 *
 * @param shared_state Pointer to stack-allocated shared state structure
 * @param result_buffer Buffer to store successful result (can be NULL if not needed)
 * @param result_buffer_size Size of the result buffer in bytes
 * @param error_buffer Buffer to store error data (can be NULL if not needed)
 * @param error_buffer_size Size of the error buffer in bytes
 * @param promise Output parameter for the promise handle (write-side)
 * @param future Output parameter for the future handle (read-side)
 *
 * @note All pointers must be non-NULL except result_buffer and error_buffer
 * @note Buffers must remain valid until promise_deinit() is called
 *
 * Thread-safety: Not thread-safe. Must be called from a single thread before
 * passing the promise/future to other threads.
 */
void promise_new(future_state_s *shared_state, void *result_buffer, size_t result_buffer_size, void *error_buffer, size_t error_buffer_size, promise_t *promise, future_t *future);

/**
 * @brief Deinitialize a promise/future pair
 *
 * Cleans up the internal state. After calling this, the promise and future
 * handles should not be used.
 *
 * @param shared_state Pointer to the shared state to deinitialize
 *
 * Thread-safety: Should only be called after all threads are done using
 * the promise and future.
 */
void promise_deinit(future_state_s *shared_state);

/**
 * @brief Fulfill a promise with a successful result
 *
 * Transitions the promise from PENDING to FULFILLED state and stores the
 * result data in the pre-allocated result buffer. This wakes up any threads
 * waiting on the corresponding future.
 *
 * A promise can only be fulfilled once. Subsequent calls will fail.
 *
 * @param promise Pointer to the promise handle
 * @param result Pointer to the result data to copy
 * @param size Size of the result data in bytes
 * @return true if successfully fulfilled, false if already completed or buffer too small
 *
 * @note The result data is copied into the pre-allocated buffer
 * @note If size exceeds result_buffer_size, the operation fails
 * @note After fulfillment, the promise cannot be reused
 *
 * Thread-safety: Thread-safe. Can be called from any thread.
 */
bool promise_fulfill(promise_t *promise, const void *result, size_t size);

/**
 * @brief Reject a promise with an error
 *
 * Transitions the promise from PENDING to REJECTED state and stores the
 * error data in the pre-allocated error buffer. This wakes up any threads
 * waiting on the corresponding future.
 *
 * A promise can only be rejected once. Subsequent calls will fail.
 *
 * @param promise Pointer to the promise handle
 * @param error Pointer to the error data to copy
 * @param size Size of the error data in bytes
 * @return true if successfully rejected, false if already completed or buffer too small
 *
 * @note The error data is copied into the pre-allocated buffer
 * @note If size exceeds error_buffer_size, the operation fails
 * @note After rejection, the promise cannot be reused
 *
 * Thread-safety: Thread-safe. Can be called from any thread.
 */
bool promise_reject(promise_t *promise, const void *error, size_t size);

/**
 * @brief Block until the future completes
 *
 * Waits indefinitely for the promise to be fulfilled or rejected. This is a
 * blocking call that will suspend the calling thread until completion.
 *
 * @param future Pointer to the future handle
 * @return The final state (FUTURE_STATE_FULFILLED or FUTURE_STATE_REJECTED)
 *
 * @note If the future is already completed, returns immediately
 * @note Uses ChibiOS binary semaphore for efficient blocking
 *
 * Thread-safety: Thread-safe. Multiple threads can wait on the same future.
 */
future_state_t future_wait(future_t *future);

/**
 * @brief Wait for future completion with a timeout
 *
 * Waits for the promise to be fulfilled or rejected, but only for the specified
 * timeout duration. If the timeout expires, returns FUTURE_STATE_PENDING.
 *
 * @param future Pointer to the future handle
 * @param timeout Timeout in ChibiOS system ticks (use TIME_INFINITE for no timeout,
 *                TIME_MS2I(ms) to convert milliseconds)
 * @return The state: FULFILLED, REJECTED, or PENDING if timeout occurred
 *
 * @note If the future is already completed, returns immediately
 * @note Timeout only applies if the future is still pending
 *
 * Thread-safety: Thread-safe. Multiple threads can wait on the same future.
 */
future_state_t future_wait_timeout(future_t *future, sysinterval_t timeout);

/**
 * @brief Check if a future is still pending (non-blocking)
 *
 * @param future Pointer to the future handle
 * @return true if the promise has not yet been fulfilled or rejected
 *
 * Thread-safety: Thread-safe.
 */
bool future_is_pending(const future_t *future);

/**
 * @brief Check if a future was fulfilled (non-blocking)
 *
 * @param future Pointer to the future handle
 * @return true if the promise was fulfilled with a successful result
 *
 * Thread-safety: Thread-safe.
 */
bool future_is_fulfilled(const future_t *future);

/**
 * @brief Check if a future was rejected (non-blocking)
 *
 * @param future Pointer to the future handle
 * @return true if the promise was rejected with an error
 *
 * Thread-safety: Thread-safe.
 */
bool future_is_rejected(const future_t *future);

/**
 * @brief Retrieve the result from a fulfilled future
 *
 * Copies the result data from the internal buffer to the provided output buffer.
 * Only succeeds if the future is in FULFILLED state.
 *
 * @param future Pointer to the future handle
 * @param result Pointer to buffer where result will be copied
 * @param size Size of the output buffer
 * @return true if result was copied successfully, false if not fulfilled or invalid params
 *
 * @note If the output buffer is smaller than the result, only size bytes are copied
 * @note This can be called multiple times to retrieve the result
 * @note Should only be called after future_wait() or after checking future_is_fulfilled()
 *
 * Thread-safety: Thread-safe. Multiple threads can retrieve the same result.
 */
bool future_get_result(const future_t *future, void *result, size_t size);

/**
 * @brief Retrieve the error from a rejected future
 *
 * Copies the error data from the internal buffer to the provided output buffer.
 * Only succeeds if the future is in REJECTED state.
 *
 * @param future Pointer to the future handle
 * @param error Pointer to buffer where error will be copied
 * @param size Size of the output buffer
 * @return true if error was copied successfully, false if not rejected or invalid params
 *
 * @note If the output buffer is smaller than the error, only size bytes are copied
 * @note This can be called multiple times to retrieve the error
 * @note Should only be called after future_wait() or after checking future_is_rejected()
 *
 * Thread-safety: Thread-safe. Multiple threads can retrieve the same error.
 */
bool future_get_error(const future_t *future, void *error, size_t size);

#ifdef __cplusplus
}
#endif
