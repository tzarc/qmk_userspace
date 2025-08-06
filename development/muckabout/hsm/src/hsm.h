// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file hsm.h
 * @brief Hierarchical State Machine Framework
 *
 * This header provides a generic, lightweight state machine implementation
 * with support for entry, exit, and execution callbacks. The framework
 * encapsulates state data within the HSM instance for cleaner API usage.
 *
 * @author Nick Brassel (@tzarc)
 * @date 2025
 */

#pragma once

#include <stdbool.h>

/**
 * @brief State identifier type for the state machine
 *
 * Represents a state in the state machine. States are typically defined
 * as enumeration values or integer constants.
 */
typedef int hsm_state_t;

/**
 * @brief State machine entry definition
 *
 * Defines the behavior for a single state in the state machine.
 * Each state can have optional entry and exit callbacks, and must
 * have an executor function that determines state transitions.
 */
typedef struct hsm_entry_t {
    /** @brief Called when entering this state (optional, can be NULL) */
    void (*entry)(void *state_data);

    /** @brief Executes state logic and returns next state (required) */
    hsm_state_t (*executor)(void *state_data);

    /** @brief Called when exiting this state (optional, can be NULL) */
    void (*exit)(void *state_data);
} hsm_entry_t;

/**
 * @brief Hierarchical State Machine instance
 *
 * Contains the complete state machine context including current state,
 * state machine definition, and associated application data.
 */
typedef struct hsm_t {
    /** @brief Current active state */
    hsm_state_t state;

    /** @brief Array of state definitions */
    const hsm_entry_t *state_machine;

    /** @brief Number of states in the state machine */
    hsm_state_t num_states;

    /** @brief Application-specific data passed to state callbacks */
    void *state_data;
} hsm_t;

/**
 * @brief Initialize a hierarchical state machine
 *
 * Sets up the state machine with the provided state definitions and
 * initializes it to the specified initial state. The entry callback
 * for the initial state is automatically called during initialization.
 *
 * @param hsm Pointer to the HSM instance to initialize
 * @param state_machine Array of state definitions (must remain valid for HSM lifetime)
 * @param num_states Number of states in the state_machine array
 * @param initial_state Starting state (must be valid index < num_states)
 * @param state_data Application data pointer passed to all state callbacks
 * @return true if initialization was successful, false on error or invalid parameters
 *
 * @note HSM structure is cleared with memset() at start of initialization for clean state
 * @note If initial_state is invalid (< 0 or >= num_states), defaults to state 0
 * @note The state_machine array must remain valid for the entire lifetime of the HSM
 * @note Returns false if initial state lacks required executor function
 */
bool hsm_init(hsm_t *hsm, const hsm_entry_t *state_machine, hsm_state_t num_states, hsm_state_t initial_state, void *state_data);

/**
 * @brief Execute one iteration of the state machine
 *
 * Calls the executor function for the current state and handles any
 * state transitions. If the executor returns a different state, the
 * exit callback for the current state is called (if present), followed
 * by the entry callback for the new state (if present).
 *
 * @param hsm Pointer to the initialized HSM instance
 * @return true if execution was successful, false on error or invalid parameters
 *
 * @note Returns false if hsm is NULL or improperly initialized
 * @note All state callbacks receive the state_data pointer from the HSM instance
 * @note Validates state bounds before and after executor calls for safety
 * @note Returns false if state executor returns invalid state bounds
 * @note Returns false if current state lacks required executor function
 */
bool hsm_execute(hsm_t *hsm);

/**
 * @brief Convenience macro for initializing a state machine with automatic array sizing
 *
 * This macro automatically calculates the number of states in the state_machine
 * array using sizeof, eliminating the need to manually specify num_states.
 *
 * @param hsm Pointer to the HSM instance to initialize
 * @param state_machine Array of state definitions (array, not pointer)
 * @param initial_state Starting state identifier
 * @param state_data Application data pointer
 *
 * @note The state_machine parameter must be an actual array, not a pointer
 * @note Equivalent to calling hsm_init() with sizeof calculation for num_states
 *
 * Example usage:
 * @code
 * hsm_entry_t my_states[] = { ... };
 * hsm_t my_hsm;
 * HSM_INIT(&my_hsm, my_states, INITIAL_STATE, &my_data);
 * @endcode
 */
#define HSM_INIT(hsm, state_machine, initial_state, state_data) hsm_init((hsm), (state_machine), (sizeof(state_machine) / sizeof(state_machine[0])), (initial_state), (state_data))
