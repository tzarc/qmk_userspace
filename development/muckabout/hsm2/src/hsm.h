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
 * @date 2024-2025
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
 * @brief Event identifier type for triggering state transitions
 *
 * Represents an event that can trigger state machine transitions.
 * Events are typically defined as enumeration values or integer constants.
 */
typedef int hsm_event_t;

/**
 * @brief Special state value to indicate no state change
 */
#define HSM_NO_TRANSITION (-1)

/**
 * @brief Transition table entry defining state transitions
 *
 * Defines a single transition from one state to another based on an event.
 * Multiple entries can exist for the same from_state with different events.
 */
typedef struct hsm_transition_t {
    /** @brief Source state for this transition */
    hsm_state_t from_state;

    /** @brief Event that triggers this transition */
    hsm_event_t event;

    /** @brief Destination state for this transition */
    hsm_state_t to_state;

    /** @brief Optional guard condition function (can be NULL for always allowed) */
    bool (*guard)(void *state_data);
} hsm_transition_t;

/**
 * @brief State machine entry definition
 *
 * Defines the behavior for a single state in the state machine.
 * Each state can have optional entry, exit, and action callbacks.
 * Transitions are now handled externally via transition tables.
 */
typedef struct hsm_entry_t {
    /** @brief Called when entering this state (optional, can be NULL) */
    void (*entry)(void *state_data);

    /** @brief Called during each execution cycle while in this state (optional, can be NULL) */
    void (*action)(void *state_data);

    /** @brief Called when exiting this state (optional, can be NULL) */
    void (*exit)(void *state_data);
} hsm_entry_t;

/**
 * @brief Hierarchical State Machine instance
 *
 * Contains the complete state machine context including current state,
 * state machine definition, transition table, and associated application data.
 */
typedef struct hsm_t {
    /** @brief Current active state */
    hsm_state_t state;

    /** @brief Array of state definitions */
    const hsm_entry_t *state_machine;

    /** @brief Number of states in the state machine */
    hsm_state_t num_states;

    /** @brief Array of transition definitions */
    const hsm_transition_t *transitions;

    /** @brief Number of transitions in the transition table */
    hsm_state_t num_transitions;

    /** @brief Application-specific data passed to state callbacks */
    void *state_data;
} hsm_t;

/**
 * @brief Initialize a hierarchical state machine
 *
 * Sets up the state machine with the provided state definitions, transition table,
 * and initializes it to the specified initial state. The entry callback
 * for the initial state is automatically called during initialization.
 *
 * @param hsm Pointer to the HSM instance to initialize
 * @param state_machine Array of state definitions (must remain valid for HSM lifetime)
 * @param num_states Number of states in the state_machine array
 * @param transitions Array of transition definitions (must remain valid for HSM lifetime)
 * @param num_transitions Number of transitions in the transitions array
 * @param initial_state Starting state (must be valid index < num_states)
 * @param state_data Application data pointer passed to all state callbacks
 * @return true if initialization was successful, false on error or invalid parameters
 *
 * @note HSM structure is cleared with memset() at start of initialization for clean state
 * @note If initial_state is invalid (< 0 or >= num_states), defaults to state 0
 * @note The state_machine and transitions arrays must remain valid for the entire lifetime of the HSM
 */
bool hsm_init(hsm_t *hsm, const hsm_entry_t *state_machine, hsm_state_t num_states, const hsm_transition_t *transitions, hsm_state_t num_transitions, hsm_state_t initial_state, void *state_data);

/**
 * @brief Trigger an event in the state machine
 *
 * First executes the current state's action function (if present), then searches
 * the transition table for a matching transition from the current state with the given event.
 * If found and guard condition passes (if present), executes the transition by calling
 * exit callback, changing state, and calling entry callback.
 *
 * @param hsm Pointer to the initialized HSM instance
 * @param event Event to trigger
 * @return true if execution was successful (regardless of whether transition occurred), false on error
 *
 * @note Returns false if hsm is NULL or improperly initialized
 * @note Multiple transitions with the same from_state and event are allowed - first match wins
 * @note Guard functions are called with the current state_data to determine if transition is allowed
 * @note State action is always executed before checking for transitions
 */
bool hsm_trigger_event(hsm_t *hsm, hsm_event_t event);

/**
 * @brief Convenience macro for initializing a state machine with automatic array sizing
 *
 * This macro automatically calculates the number of states and transitions in the arrays
 * using sizeof, eliminating the need to manually specify counts.
 *
 * @param hsm Pointer to the HSM instance to initialize
 * @param state_machine Array of state definitions (array, not pointer)
 * @param transitions Array of transition definitions (array, not pointer)
 * @param initial_state Starting state identifier
 * @param state_data Application data pointer
 *
 * @note The state_machine and transitions parameters must be actual arrays, not pointers
 * @note Equivalent to calling hsm_init() with sizeof calculations for array lengths
 *
 * Example usage:
 * @code
 * hsm_entry_t my_states[] = { ... };
 * hsm_transition_t my_transitions[] = { ... };
 * hsm_t my_hsm;
 * HSM_INIT(&my_hsm, my_states, my_transitions, INITIAL_STATE, &my_data);
 * @endcode
 */
#define HSM_INIT(hsm, state_machine, transitions, initial_state, state_data) hsm_init((hsm), (state_machine), (sizeof(state_machine) / sizeof(state_machine[0])), (transitions), (sizeof(transitions) / sizeof(transitions[0])), (initial_state), (state_data))
