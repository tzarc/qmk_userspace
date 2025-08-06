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

bool hsm_init(hsm_t *hsm, const hsm_entry_t *state_machine, hsm_state_t num_states, hsm_state_t initial_state, void *state_data) {
    if (!hsm) {
        return false;
    }

    // Clear the HSM structure to ensure clean initialization
    memset(hsm, 0, sizeof(*hsm));

    if (!state_machine || num_states <= 0) {
        return false;
    }

    hsm->state         = initial_state;
    hsm->state_machine = state_machine;
    hsm->num_states    = num_states;
    hsm->state_data    = state_data;

    // Validate that ALL states have executor functions
    for (hsm_state_t i = 0; i < num_states; i++) {
        if (!state_machine[i].executor) {
            return false;
        }
    }

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

bool hsm_execute(hsm_t *hsm) {
    if (!hsm_is_valid_state(hsm, hsm ? hsm->state : -1)) {
        return false;
    }

    // Check that executor function exists for current state
    if (!hsm->state_machine[hsm->state].executor) {
        return false;
    }

    hsm_state_t prev_state = hsm->state;
    hsm_state_t next_state = (*hsm->state_machine[prev_state].executor)(hsm->state_data);

    if (!hsm_is_valid_state(hsm, next_state)) {
        return false;
    }

    if (next_state != prev_state) {
        if (hsm->state_machine[prev_state].exit) {
            (*hsm->state_machine[prev_state].exit)(hsm->state_data);
        }

        hsm->state = next_state;

        if (hsm->state_machine[next_state].entry) {
            (*hsm->state_machine[next_state].entry)(hsm->state_data);
        }
    }

    return true;
}
