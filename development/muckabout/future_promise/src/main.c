// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file main.c
 * @brief Example usage of the promise/future pattern
 *
 * This file demonstrates various usage patterns for the promise/future library:
 * 1. Basic fulfillment - happy path with successful result
 * 2. Promise rejection - error handling path
 * 3. Timeout waiting - non-blocking wait with timeout
 * 4. Polling pattern - checking state without blocking
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ch.h"
#include "future.h"

//==============================================================================
// Worker Threads
//==============================================================================

/**
 * @brief Worker thread that fulfills a promise after simulated work
 *
 * This represents a typical producer thread that performs some computation
 * and then fulfills the promise with a result.
 */
static THD_WORKING_AREA(waWorkerThread, 512);
static THD_FUNCTION(WorkerThread, arg) {
    promise_t *promise = (promise_t *)arg;

    printf("Worker: Starting computation...\n");
    chThdSleepMilliseconds(2000); // Simulate 2 seconds of work

    int result = 42;
    printf("Worker: Computation complete, fulfilling promise with %d\n", result);
    promise_fulfill(promise, &result, sizeof(result));

    return MSG_OK;
}

/**
 * @brief Worker thread that rejects a promise after a failure
 *
 * This represents a producer thread that encounters an error and
 * rejects the promise with an error message.
 */
static THD_WORKING_AREA(waErrorThread, 512);
static THD_FUNCTION(ErrorThread, arg) {
    promise_t *promise = (promise_t *)arg;

    printf("Error worker: Starting operation...\n");
    chThdSleepMilliseconds(1000); // Simulate 1 second of work before failure

    const char *error_msg = "Operation failed";
    printf("Error worker: Operation failed, rejecting promise\n");
    promise_reject(promise, error_msg, strlen(error_msg) + 1);

    return MSG_OK;
}

//==============================================================================
// Example 1: Basic Promise/Future Usage
//==============================================================================

/**
 * @brief Demonstrates basic promise/future pattern
 *
 * Pattern:
 * 1. Main thread creates promise/future pair
 * 2. Worker thread receives promise (write-side)
 * 3. Main thread keeps future (read-side)
 * 4. Worker fulfills promise after work completes
 * 5. Main thread blocks on future_wait() until completion
 * 6. Main thread retrieves result
 *
 * Key concepts:
 * - Stack-allocated shared state and buffers (no malloc)
 * - Type-safe separation of write-side (promise) and read-side (future)
 * - Blocking wait for completion
 */
void example_basic_future(void) {
    printf("\n=== Basic Promise/Future Example ===\n");

    // Allocate all structures on the stack
    future_state_s shared_state;
    promise_t      promise;
    future_t       future;
    int            result_buffer;
    char           error_buffer[256];

    // Create promise/future pair with pre-allocated buffers
    promise_new(&shared_state, &result_buffer, sizeof(result_buffer), error_buffer, sizeof(error_buffer), &promise, &future);

    // Spawn worker thread, passing promise (write-side)
    thread_t *worker = chThdCreateStatic(waWorkerThread, sizeof(waWorkerThread), NORMALPRIO, WorkerThread, &promise);

    // Main thread blocks until worker completes
    printf("Main: Waiting for result...\n");
    future_state_t state = future_wait(&future);

    // Check if promise was fulfilled and retrieve result
    if (state == FUTURE_STATE_FULFILLED) {
        int result;
        if (future_get_result(&future, &result, sizeof(result))) {
            printf("Main: Got result: %d\n", result);
        }
    }

    // Clean up
    chThdWait(worker);
    promise_deinit(&shared_state);
}

//==============================================================================
// Example 2: Promise Rejection
//==============================================================================

/**
 * @brief Demonstrates error handling with promise rejection
 *
 * Pattern:
 * 1. Worker encounters an error condition
 * 2. Worker rejects promise with error data
 * 3. Main thread detects rejection via future_wait()
 * 4. Main thread retrieves error information
 *
 * Key concepts:
 * - Error handling through rejection path
 * - Distinguishing between fulfilled and rejected states
 * - Retrieving error data from rejected future
 */
void example_rejected_future(void) {
    printf("\n=== Rejected Promise Example ===\n");

    future_state_s shared_state;
    promise_t      promise;
    future_t       future;
    int            result_buffer;
    char           error_buffer[256];

    promise_new(&shared_state, &result_buffer, sizeof(result_buffer), error_buffer, sizeof(error_buffer), &promise, &future);

    // Spawn worker that will reject the promise
    thread_t *worker = chThdCreateStatic(waErrorThread, sizeof(waErrorThread), NORMALPRIO, ErrorThread, &promise);

    printf("Main: Waiting for result...\n");
    future_state_t state = future_wait(&future);

    // Check if promise was rejected and retrieve error
    if (state == FUTURE_STATE_REJECTED) {
        char error[256];
        if (future_get_error(&future, error, sizeof(error))) {
            printf("Main: Got error: %s\n", error);
        }
    }

    chThdWait(worker);
    promise_deinit(&shared_state);
}

//==============================================================================
// Example 3: Timeout-based Waiting
//==============================================================================

/**
 * @brief Demonstrates waiting with timeout
 *
 * Pattern:
 * 1. Main thread waits with short timeout (500ms)
 * 2. Worker takes longer (2000ms), so timeout occurs
 * 3. Main thread detects timeout (PENDING state)
 * 4. Main thread decides to wait indefinitely
 * 5. Eventually receives result
 *
 * Key concepts:
 * - Non-blocking wait with timeout using future_wait_timeout()
 * - Detecting timeout via PENDING state return
 * - Combining timeout wait with full wait
 * - Allows implementation of responsive UIs or watchdog patterns
 */
void example_timeout_future(void) {
    printf("\n=== Timeout Future Example ===\n");

    future_state_s shared_state;
    promise_t      promise;
    future_t       future;
    int            result_buffer;
    char           error_buffer[256];

    promise_new(&shared_state, &result_buffer, sizeof(result_buffer), error_buffer, sizeof(error_buffer), &promise, &future);

    thread_t *worker = chThdCreateStatic(waWorkerThread, sizeof(waWorkerThread), NORMALPRIO, WorkerThread, &promise);

    // Try waiting with 500ms timeout (worker takes 2000ms)
    printf("Main: Waiting for result with 500ms timeout...\n");
    future_state_t state = future_wait_timeout(&future, TIME_MS2I(500));

    // Timeout occurred - promise still pending
    if (state == FUTURE_STATE_PENDING) {
        printf("Main: Timeout occurred, still waiting...\n");
        // Decide to wait indefinitely
        state = future_wait(&future);
    }

    // Eventually get the result
    if (state == FUTURE_STATE_FULFILLED) {
        int result;
        if (future_get_result(&future, &result, sizeof(result))) {
            printf("Main: Eventually got result: %d\n", result);
        }
    }

    chThdWait(worker);
    promise_deinit(&shared_state);
}

//==============================================================================
// Example 4: Polling Pattern
//==============================================================================

/**
 * @brief Demonstrates polling instead of blocking
 *
 * Pattern:
 * 1. Main thread periodically checks future state
 * 2. Performs other work between checks (sleep in this example)
 * 3. Once completed, retrieves result
 *
 * Key concepts:
 * - Non-blocking state checks with future_is_pending()
 * - Allows main thread to do other work while waiting
 * - Useful for event loops or when blocking is not desirable
 * - Less efficient than blocking wait (busy polling with sleep)
 */
void example_polling_future(void) {
    printf("\n=== Polling Future Example ===\n");

    future_state_s shared_state;
    promise_t      promise;
    future_t       future;
    int            result_buffer;
    char           error_buffer[256];

    promise_new(&shared_state, &result_buffer, sizeof(result_buffer), error_buffer, sizeof(error_buffer), &promise, &future);

    thread_t *worker = chThdCreateStatic(waWorkerThread, sizeof(waWorkerThread), NORMALPRIO, WorkerThread, &promise);

    // Poll for completion instead of blocking
    printf("Main: Polling for completion...\n");
    while (future_is_pending(&future)) {
        printf("Main: Still pending...\n");
        chThdSleepMilliseconds(500); // Could do other work here
    }

    // Check final state and retrieve result
    if (future_is_fulfilled(&future)) {
        int result;
        if (future_get_result(&future, &result, sizeof(result))) {
            printf("Main: Polled result: %d\n", result);
        }
    }

    chThdWait(worker);
    promise_deinit(&shared_state);
}

//==============================================================================
// Main Entry Point
//==============================================================================

int main(void) {
    // Initialize ChibiOS
    chSysInit();

    // Run all examples sequentially
    example_basic_future();
    example_rejected_future();
    example_timeout_future();
    example_polling_future();

    printf("\n=== All examples completed ===\n");

    return 0;
}
