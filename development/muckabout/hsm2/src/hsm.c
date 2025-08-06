// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "hsm.h"
#include <string.h>

/**
 * @brief Internal function to validate state bounds
 *
 * @param hsm Pointer to HSM instance
 * @param state State to validate
 * @return true if state is valid, false otherwise
 */
static bool hsm_is_valid_state(const hsm_t *hsm, hsm_state_t state) {
    return hsm && hsm->state_machine && state >= 0 && state < hsm->num_states;
}

bool hsm_init(hsm_t *hsm, const hsm_entry_t *state_machine, hsm_state_t num_states, const hsm_transition_t *transitions, hsm_state_t num_transitions, hsm_state_t initial_state, void *state_data) {
    if (!hsm) {
        return false;
    }

    // Clear the HSM structure to ensure clean initialization
    memset(hsm, 0, sizeof(*hsm));

    if (!state_machine || num_states <= 0) {
        return false;
    }

    hsm->state           = initial_state;
    hsm->state_machine   = state_machine;
    hsm->num_states      = num_states;
    hsm->transitions     = transitions;
    hsm->num_transitions = num_transitions;
    hsm->state_data      = state_data;

    // Ensure the initial state is valid
    if (!hsm_is_valid_state(hsm, initial_state)) {
        hsm->state = 0; // Default to the first state if invalid
    }

    // Invoke the entry function for the initial state
    if (hsm->state_machine[hsm->state].entry) {
        (*hsm->state_machine[hsm->state].entry)(hsm->state_data);
    }

    return true;
}

bool hsm_trigger_event(hsm_t *hsm, hsm_event_t event) {
    if (!hsm_is_valid_state(hsm, hsm ? hsm->state : -1)) {
        return false;
    }

    // First execute the current state's action function if it exists
    if (hsm->state_machine[hsm->state].action) {
        (*hsm->state_machine[hsm->state].action)(hsm->state_data);
    }

    // Search for a matching transition
    for (hsm_state_t i = 0; i < hsm->num_transitions; i++) {
        const hsm_transition_t *transition = &hsm->transitions[i];

        if (transition->from_state == hsm->state && transition->event == event) {
            // Check guard condition if present
            if (transition->guard && !transition->guard(hsm->state_data)) {
                continue; // Guard condition failed, try next transition
            }

            // Validate target state
            if (!hsm_is_valid_state(hsm, transition->to_state)) {
                return false;
            }

            // Execute the transition
            hsm_state_t prev_state = hsm->state;

            // Call exit callback for current state
            if (hsm->state_machine[prev_state].exit) {
                (*hsm->state_machine[prev_state].exit)(hsm->state_data);
            }

            // Change state
            hsm->state = transition->to_state;

            // Call entry callback for new state
            if (hsm->state_machine[hsm->state].entry) {
                (*hsm->state_machine[hsm->state].entry)(hsm->state_data);
            }

            return true; // Transition executed successfully
        }
    }

    // No matching transition found - this is not an error
    return true;
}
