// Copyright 2024-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "future.h"
#include <string.h>

//==============================================================================
// Initialization and Cleanup
//==============================================================================

void promise_new(future_state_s *shared_state, void *result_buffer, size_t result_buffer_size, void *error_buffer, size_t error_buffer_size, promise_t *promise, future_t *future) {
    if (!shared_state || !promise || !future) return;

    // Initialize ChibiOS synchronization primitives
    chMtxObjectInit(&shared_state->mutex);         // Protects state transitions and data
    chBSemObjectInit(&shared_state->signal, true); // Binary semaphore starts taken (will be signaled on completion)

    // Initialize state to pending
    shared_state->state = FUTURE_STATE_PENDING;

    // Store pointers to pre-allocated buffers (no dynamic allocation)
    shared_state->result_buffer      = result_buffer;
    shared_state->result_buffer_size = result_buffer_size;
    shared_state->error_buffer       = error_buffer;
    shared_state->error_buffer_size  = error_buffer_size;
    shared_state->result_size        = 0;
    shared_state->error_size         = 0;

    // Both promise and future point to the same shared state
    promise->shared = shared_state;
    future->shared  = shared_state;
}

void promise_deinit(future_state_s *shared_state) {
    if (!shared_state) return;

    // Lock to ensure no concurrent access during cleanup
    chMtxLock(&shared_state->mutex);

    // Reset state (doesn't free buffers - they're user-managed)
    shared_state->state       = FUTURE_STATE_PENDING;
    shared_state->result_size = 0;
    shared_state->error_size  = 0;

    chMtxUnlock(&shared_state->mutex);
}

//==============================================================================
// Promise Operations (Write-side)
//==============================================================================

bool promise_fulfill(promise_t *promise, const void *result, size_t size) {
    if (!promise || !promise->shared) return false;

    future_state_s *shared = promise->shared;

    // Critical section: check state and copy result atomically
    chMtxLock(&shared->mutex);

    // Can only fulfill a pending promise (prevents double-fulfill/reject)
    if (shared->state != FUTURE_STATE_PENDING) {
        chMtxUnlock(&shared->mutex);
        return false;
    }

    // Copy result data into pre-allocated buffer
    if (result && size > 0) {
        // Fail if buffer is too small
        if (!shared->result_buffer || size > shared->result_buffer_size) {
            chMtxUnlock(&shared->mutex);
            return false;
        }
        memcpy(shared->result_buffer, result, size);
        shared->result_size = size;
    }

    // Transition to fulfilled state
    shared->state = FUTURE_STATE_FULFILLED;

    // Release lock before signaling (standard pattern to avoid priority inversion)
    chMtxUnlock(&shared->mutex);

    // Signal completion - wakes up any threads waiting on the future
    // Using binary semaphore ensures all waiters are woken
    chBSemSignal(&shared->signal);

    return true;
}

bool promise_reject(promise_t *promise, const void *error, size_t size) {
    if (!promise || !promise->shared) return false;

    future_state_s *shared = promise->shared;

    // Critical section: check state and copy error atomically
    chMtxLock(&shared->mutex);

    // Can only reject a pending promise (prevents double-fulfill/reject)
    if (shared->state != FUTURE_STATE_PENDING) {
        chMtxUnlock(&shared->mutex);
        return false;
    }

    // Copy error data into pre-allocated buffer
    if (error && size > 0) {
        // Fail if buffer is too small
        if (!shared->error_buffer || size > shared->error_buffer_size) {
            chMtxUnlock(&shared->mutex);
            return false;
        }
        memcpy(shared->error_buffer, error, size);
        shared->error_size = size;
    }

    // Transition to rejected state
    shared->state = FUTURE_STATE_REJECTED;

    // Release lock before signaling (standard pattern to avoid priority inversion)
    chMtxUnlock(&shared->mutex);

    // Signal completion - wakes up any threads waiting on the future
    chBSemSignal(&shared->signal);

    return true;
}

//==============================================================================
// Future Operations (Read-side) - Blocking
//==============================================================================

future_state_t future_wait(future_t *future) {
    if (!future || !future->shared) return FUTURE_STATE_PENDING;

    future_state_s *shared = future->shared;

    // Quick check: is it already completed?
    chMtxLock(&shared->mutex);
    future_state_t state = shared->state;
    chMtxUnlock(&shared->mutex);

    // If still pending, block until signaled
    if (state == FUTURE_STATE_PENDING) {
        // Block on binary semaphore until promise is fulfilled/rejected
        // This is efficient - thread is suspended until signal
        chBSemWait(&shared->signal);

        // Re-check state after waking up
        chMtxLock(&shared->mutex);
        state = shared->state;
        chMtxUnlock(&shared->mutex);
    }

    return state;
}

future_state_t future_wait_timeout(future_t *future, sysinterval_t timeout) {
    if (!future || !future->shared) return FUTURE_STATE_PENDING;

    future_state_s *shared = future->shared;

    // Quick check: is it already completed?
    chMtxLock(&shared->mutex);
    future_state_t state = shared->state;
    chMtxUnlock(&shared->mutex);

    // If still pending, block with timeout
    if (state == FUTURE_STATE_PENDING) {
        // Block on binary semaphore with timeout
        msg_t result = chBSemWaitTimeout(&shared->signal, timeout);

        // If timeout occurred, return pending state
        if (result == MSG_TIMEOUT) {
            return FUTURE_STATE_PENDING;
        }

        // Otherwise, promise was fulfilled/rejected - get the new state
        chMtxLock(&shared->mutex);
        state = shared->state;
        chMtxUnlock(&shared->mutex);
    }

    return state;
}

//==============================================================================
// Future Operations (Read-side) - Non-blocking State Checks
//==============================================================================

bool future_is_pending(const future_t *future) {
    if (!future || !future->shared) return false;

    future_state_s *shared = future->shared;

    // Thread-safe state check
    chMtxLock((mutex_t *)&shared->mutex);
    bool pending = (shared->state == FUTURE_STATE_PENDING);
    chMtxUnlock((mutex_t *)&shared->mutex);

    return pending;
}

bool future_is_fulfilled(const future_t *future) {
    if (!future || !future->shared) return false;

    future_state_s *shared = future->shared;

    // Thread-safe state check
    chMtxLock((mutex_t *)&shared->mutex);
    bool fulfilled = (shared->state == FUTURE_STATE_FULFILLED);
    chMtxUnlock((mutex_t *)&shared->mutex);

    return fulfilled;
}

bool future_is_rejected(const future_t *future) {
    if (!future || !future->shared) return false;

    future_state_s *shared = future->shared;

    // Thread-safe state check
    chMtxLock((mutex_t *)&shared->mutex);
    bool rejected = (shared->state == FUTURE_STATE_REJECTED);
    chMtxUnlock((mutex_t *)&shared->mutex);

    return rejected;
}

//==============================================================================
// Future Operations (Read-side) - Value Retrieval
//==============================================================================

bool future_get_result(const future_t *future, void *result, size_t size) {
    if (!future || !future->shared || !result || size == 0) return false;

    future_state_s *shared = future->shared;

    // Lock to safely read result data
    chMtxLock((mutex_t *)&shared->mutex);

    // Can only get result from fulfilled future
    if (shared->state != FUTURE_STATE_FULFILLED || !shared->result_buffer || shared->result_size == 0) {
        chMtxUnlock((mutex_t *)&shared->mutex);
        return false;
    }

    // Copy result to output buffer (copy only what fits)
    size_t copy_size = (size < shared->result_size) ? size : shared->result_size;
    memcpy(result, shared->result_buffer, copy_size);

    chMtxUnlock((mutex_t *)&shared->mutex);

    return true;
}

bool future_get_error(const future_t *future, void *error, size_t size) {
    if (!future || !future->shared || !error || size == 0) return false;

    future_state_s *shared = future->shared;

    // Lock to safely read error data
    chMtxLock((mutex_t *)&shared->mutex);

    // Can only get error from rejected future
    if (shared->state != FUTURE_STATE_REJECTED || !shared->error_buffer || shared->error_size == 0) {
        chMtxUnlock((mutex_t *)&shared->mutex);
        return false;
    }

    // Copy error to output buffer (copy only what fits)
    size_t copy_size = (size < shared->error_size) ? size : shared->error_size;
    memcpy(error, shared->error_buffer, copy_size);

    chMtxUnlock((mutex_t *)&shared->mutex);

    return true;
}
