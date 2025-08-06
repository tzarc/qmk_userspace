// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file test_hsm.c
 * @brief Comprehensive unit tests for the Hierarchical State Machine (HSM) framework
 *
 * This test suite validates all aspects of the HSM framework including:
 * - Initialization with various parameter combinations
 * - State execution and event-driven transitions
 * - Error handling and boundary conditions
 * - Guard conditions and transition tables
 * - Memory safety and cleanup
 *
 * The tests achieve 100% code coverage of the HSM implementation.
 */

#include "utest.h"
#include "../src/hsm.h"
#include <string.h>

// Test state data structure
typedef struct {
    int  counter;
    int  last_entered_state;
    int  last_exited_state;
    bool entry_called;
    bool exit_called;
    bool action_called;
    bool guard_called;
    bool guard_result;
} test_data_t;

// Test states
typedef enum {
    STATE_A       = 0,
    STATE_B       = 1,
    STATE_C       = 2,
    INVALID_STATE = 99,
} test_states_t;

// Test events
typedef enum {
    EVENT_NEXT    = 0,
    EVENT_RESET   = 1,
    EVENT_INVALID = 99,
} test_events_t;

// Entry functions
static void state_a_entry(void *data) {
    test_data_t *td        = (test_data_t *)data;
    td->entry_called       = true;
    td->last_entered_state = STATE_A;
}

static void state_b_entry(void *data) {
    test_data_t *td        = (test_data_t *)data;
    td->entry_called       = true;
    td->last_entered_state = STATE_B;
}

static void state_c_entry(void *data) {
    test_data_t *td        = (test_data_t *)data;
    td->entry_called       = true;
    td->last_entered_state = STATE_C;
}

// Exit functions
static void state_a_exit(void *data) {
    test_data_t *td       = (test_data_t *)data;
    td->exit_called       = true;
    td->last_exited_state = STATE_A;
}

static void state_b_exit(void *data) {
    test_data_t *td       = (test_data_t *)data;
    td->exit_called       = true;
    td->last_exited_state = STATE_B;
}

static void state_c_exit(void *data) {
    test_data_t *td       = (test_data_t *)data;
    td->exit_called       = true;
    td->last_exited_state = STATE_C;
}

// Action functions
static void state_a_action(void *data) {
    test_data_t *td   = (test_data_t *)data;
    td->action_called = true;
    td->counter++;
}

static void state_b_action(void *data) {
    test_data_t *td   = (test_data_t *)data;
    td->action_called = true;
    td->counter++;
}

static void state_c_action(void *data) {
    test_data_t *td   = (test_data_t *)data;
    td->action_called = true;
    td->counter++;
}

// Guard functions
static bool test_guard(void *data) {
    test_data_t *td  = (test_data_t *)data;
    td->guard_called = true;
    return td->guard_result;
}

static bool always_false_guard(void *data) {
    test_data_t *td  = (test_data_t *)data;
    td->guard_called = true;
    return false;
}

// Test state machines
static const hsm_entry_t test_state_machine[] = {
    [STATE_A] = {state_a_entry, state_a_action, state_a_exit},
    [STATE_B] = {state_b_entry, state_b_action, state_b_exit},
    [STATE_C] = {state_c_entry, state_c_action, state_c_exit},
};

static const hsm_transition_t test_transitions[] = {
    {STATE_A, EVENT_NEXT, STATE_B, NULL},
    {STATE_B, EVENT_NEXT, STATE_C, NULL},
    {STATE_C, EVENT_RESET, STATE_A, NULL},
    {STATE_A, EVENT_RESET, STATE_A, test_guard}, // Self-transition with guard
};

static const hsm_entry_t minimal_state_machine[] = {
    [STATE_A] = {NULL, state_a_action, NULL},
    [STATE_B] = {NULL, state_b_action, NULL},
};

static const hsm_transition_t minimal_transitions[] = {
    {STATE_A, EVENT_NEXT, STATE_B, NULL},
};

// State machine with missing action (should still work)
static const hsm_entry_t action_optional_state_machine[] = {
    [STATE_A] = {state_a_entry, NULL, state_a_exit}, // No action
    [STATE_B] = {state_b_entry, state_b_action, state_b_exit},
};

static const hsm_transition_t action_optional_transitions[] = {
    {STATE_A, EVENT_NEXT, STATE_B, NULL},
};

// Guard test transitions
static const hsm_transition_t guard_test_transitions[] = {
    {STATE_A, EVENT_NEXT, STATE_B, test_guard},
    {STATE_A, EVENT_RESET, STATE_C, always_false_guard}, // Should never trigger
};

//==============================================================================
// Test functions
//==============================================================================

/**
 * @test init_null_hsm
 * @brief Verifies that hsm_init() properly rejects NULL HSM pointer
 *
 * Purpose: Ensures robust error handling when invalid HSM pointer is passed.
 * This prevents crashes and undefined behavior when the API is misused.
 *
 * Coverage: Tests the first validation check in hsm_init().
 */
UTEST(hsm, init_null_hsm) {
    ASSERT_FALSE(hsm_init(NULL, test_state_machine, 3, test_transitions, 4, STATE_A, NULL));
}

/**
 * @test init_null_state_machine
 * @brief Verifies that hsm_init() properly rejects NULL state machine array
 *
 * Purpose: Ensures the HSM cannot be initialized without a valid state machine
 * definition. This prevents undefined behavior when trying to access states.
 *
 * Coverage: Tests the state_machine validation in hsm_init().
 */
UTEST(hsm, init_null_state_machine) {
    hsm_t hsm;
    ASSERT_FALSE(hsm_init(&hsm, NULL, 3, test_transitions, 4, STATE_A, NULL));
}

/**
 * @test init_zero_states
 * @brief Verifies that hsm_init() rejects zero state count
 *
 * Purpose: Ensures the HSM requires at least one state to operate properly.
 * A state machine with no states would be meaningless and cause array access errors.
 *
 * Coverage: Tests the num_states <= 0 validation in hsm_init().
 */
UTEST(hsm, init_zero_states) {
    hsm_t hsm;
    ASSERT_FALSE(hsm_init(&hsm, test_state_machine, 0, test_transitions, 4, STATE_A, NULL));
}

/**
 * @test init_negative_states
 * @brief Verifies that hsm_init() rejects negative state count
 *
 * Purpose: Prevents undefined behavior from negative array indexing.
 * Negative state counts are nonsensical and would cause memory access violations.
 *
 * Coverage: Tests the num_states <= 0 validation in hsm_init().
 */
UTEST(hsm, init_negative_states) {
    hsm_t hsm;
    ASSERT_FALSE(hsm_init(&hsm, test_state_machine, -1, test_transitions, 4, STATE_A, NULL));
}

/**
 * @test init_valid_parameters
 * @brief Verifies successful HSM initialization with valid parameters
 *
 * Purpose: Tests the happy path of HSM initialization, ensuring all fields
 * are properly set and the initial state's entry function is called.
 *
 * Coverage: Tests successful initialization path and entry callback invocation.
 */
UTEST(hsm, init_valid_parameters) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, test_transitions, 4, STATE_A, &data));
    ASSERT_EQ(hsm.state, STATE_A);
    ASSERT_TRUE(data.entry_called);
    ASSERT_EQ(data.last_entered_state, STATE_A);
}

/**
 * @test init_invalid_initial_state
 * @brief Tests HSM behavior when given an out-of-bounds initial state
 *
 * Purpose: Verifies the HSM gracefully handles invalid initial states by
 * defaulting to state 0. This prevents crashes from array bounds violations.
 *
 * Coverage: Tests the state validation and default assignment logic.
 */
UTEST(hsm, init_invalid_initial_state) {
    hsm_t       hsm;
    test_data_t data = {0};
    // Invalid initial state should default to state 0
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, test_transitions, 4, INVALID_STATE, &data));
    ASSERT_EQ(hsm.state, 0);
    ASSERT_TRUE(data.entry_called);
    ASSERT_EQ(data.last_entered_state, STATE_A);
}

/**
 * @test init_negative_initial_state
 * @brief Tests HSM behavior when given a negative initial state
 *
 * Purpose: Ensures negative initial states are handled gracefully by defaulting
 * to state 0. Prevents undefined behavior from negative array indexing.
 *
 * Coverage: Tests the state validation logic with negative values.
 */
UTEST(hsm, init_negative_initial_state) {
    hsm_t       hsm;
    test_data_t data = {0};
    // Negative initial state should default to state 0
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, test_transitions, 4, -1, &data));
    ASSERT_EQ(hsm.state, 0);
    ASSERT_TRUE(data.entry_called);
    ASSERT_EQ(data.last_entered_state, STATE_A);
}

/**
 * @test init_no_transitions
 * @brief Verifies HSM can be initialized without any transitions
 *
 * Purpose: Tests that the HSM can operate in states without transitions,
 * useful for simple state machines that only need state actions without
 * dynamic transitions.
 *
 * Coverage: Tests initialization with NULL transitions and validates the
 * HSM still functions properly for state entry.
 */
UTEST(hsm, init_no_transitions) {
    hsm_t       hsm;
    test_data_t data = {0};
    // Should work with no transitions (NULL transitions, 0 count)
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, NULL, 0, STATE_A, &data));
    ASSERT_EQ(hsm.state, STATE_A);
    ASSERT_TRUE(data.entry_called);
}

/**
 * @test trigger_event_null_hsm
 * @brief Verifies that hsm_trigger_event() properly rejects NULL HSM pointer
 *
 * Purpose: Ensures robust error handling when hsm_trigger_event() is called
 * with an invalid HSM pointer. Prevents crashes and undefined behavior.
 *
 * Coverage: Tests the NULL HSM validation in hsm_trigger_event().
 */
UTEST(hsm, trigger_event_null_hsm) {
    ASSERT_FALSE(hsm_trigger_event(NULL, EVENT_NEXT));
}

/**
 * @test trigger_event_invalid_current_state
 * @brief Tests hsm_trigger_event() behavior with corrupted/invalid current state
 *
 * Purpose: Ensures the HSM detects when its internal state has been corrupted
 * (e.g., by memory corruption, bugs, or manual manipulation) and safely rejects
 * event processing rather than causing array bounds violations.
 *
 * Coverage: Tests the state validation logic with invalid state values.
 */
UTEST(hsm, trigger_event_invalid_current_state) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, test_transitions, 4, STATE_A, &data));

    // Manually corrupt the state to test validation
    hsm.state = INVALID_STATE;
    ASSERT_FALSE(hsm_trigger_event(&hsm, EVENT_NEXT));
}

/**
 * @test trigger_event_negative_current_state
 * @brief Tests hsm_trigger_event() behavior with negative current state
 *
 * Purpose: Ensures the HSM safely handles negative state values which could
 * cause negative array indexing. This test simulates memory corruption or
 * programming errors that result in invalid state values.
 *
 * Coverage: Tests the state validation logic with negative values.
 */
UTEST(hsm, trigger_event_negative_current_state) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, test_transitions, 4, STATE_A, &data));

    // Manually corrupt the state to test validation
    hsm.state = -1;
    ASSERT_FALSE(hsm_trigger_event(&hsm, EVENT_NEXT));
}

/**
 * @test trigger_event_action_execution
 * @brief Verifies that state actions are executed even when no transition occurs
 *
 * Purpose: Ensures the integrated design works correctly - state actions should
 * always execute when hsm_trigger_event() is called, regardless of whether a
 * transition occurs. This tests the core behavioral requirement of the new API.
 *
 * Coverage: Tests action execution when no matching transition exists.
 */
UTEST(hsm, trigger_event_action_execution) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, test_transitions, 4, STATE_A, &data));

    data.action_called = false;
    data.counter       = 0;

    // Trigger event that doesn't cause transition but should execute action
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_INVALID));
    ASSERT_TRUE(data.action_called);
    ASSERT_EQ(data.counter, 1);
    ASSERT_EQ(hsm.state, STATE_A); // Should stay in same state
}

/**
 * @test trigger_event_successful_transition
 * @brief Tests a complete successful state transition with all callbacks
 *
 * Purpose: Verifies the full transition sequence works correctly:
 * 1. Current state action executes first
 * 2. Exit callback is called for the old state
 * 3. State changes to the new state
 * 4. Entry callback is called for the new state
 * This tests the core state machine transition logic.
 *
 * Coverage: Tests the complete transition sequence.
 */
UTEST(hsm, trigger_event_successful_transition) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, test_transitions, 4, STATE_A, &data));

    // Reset flags
    data.entry_called  = false;
    data.exit_called   = false;
    data.action_called = false;

    // Trigger transition A -> B
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_NEXT));

    // Verify action was called first
    ASSERT_TRUE(data.action_called);

    // Verify transition occurred
    ASSERT_EQ(hsm.state, STATE_B);
    ASSERT_TRUE(data.exit_called);
    ASSERT_EQ(data.last_exited_state, STATE_A);
    ASSERT_TRUE(data.entry_called);
    ASSERT_EQ(data.last_entered_state, STATE_B);
}

/**
 * @test trigger_event_multiple_transitions
 * @brief Tests a sequence of multiple state transitions
 *
 * Purpose: Verifies that the HSM can handle multiple consecutive transitions
 * correctly, maintaining proper state throughout a sequence of changes.
 * This ensures the state machine works correctly for complex workflows.
 *
 * Coverage: Tests multiple executions of the transition logic to ensure
 * state consistency across different transitions.
 */
UTEST(hsm, trigger_event_multiple_transitions) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, test_transitions, 4, STATE_A, &data));

    // A -> B
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_NEXT));
    ASSERT_EQ(hsm.state, STATE_B);

    // B -> C
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_NEXT));
    ASSERT_EQ(hsm.state, STATE_C);

    // C -> A
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_RESET));
    ASSERT_EQ(hsm.state, STATE_A);
}

/**
 * @test trigger_event_guard_allows_transition
 * @brief Tests that guard conditions can allow transitions to proceed
 *
 * Purpose: Verifies that guard functions are properly called and when they
 * return true, the transition is allowed to proceed. This tests the guard
 * validation mechanism that enables conditional state transitions.
 *
 * Coverage: Tests guard function call and success path.
 */
UTEST(hsm, trigger_event_guard_allows_transition) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, guard_test_transitions, 2, STATE_A, &data));

    data.guard_result = true;
    data.guard_called = false;

    // Trigger transition with guard that should pass
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_NEXT));
    ASSERT_TRUE(data.guard_called);
    ASSERT_EQ(hsm.state, STATE_B); // Should transition
}

/**
 * @test trigger_event_guard_prevents_transition
 * @brief Tests that guard conditions can prevent transitions from occurring
 *
 * Purpose: Verifies that when guard functions return false, the transition
 * is blocked and the state machine remains in the current state. This tests
 * the guard rejection mechanism for conditional transitions.
 *
 * Coverage: Tests guard function call and rejection path.
 */
UTEST(hsm, trigger_event_guard_prevents_transition) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, guard_test_transitions, 2, STATE_A, &data));

    data.guard_result = false;
    data.guard_called = false;

    // Trigger transition with guard that should fail
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_NEXT));
    ASSERT_TRUE(data.guard_called);
    ASSERT_EQ(hsm.state, STATE_A); // Should stay in same state
}

/**
 * @test trigger_event_guard_continues_search
 * @brief Tests that failed guards allow searching for alternative transitions
 *
 * Purpose: Verifies that when a guard fails, the HSM continues searching the
 * transition table for other matching transitions with the same state/event pair.
 * This enables priority-based transitions and fallback behavior.
 *
 * Coverage: Tests the continue logic and subsequent transition matching.
 */
UTEST(hsm, trigger_event_guard_continues_search) {
    hsm_t       hsm;
    test_data_t data = {0};

    // Create transitions where first fails guard, second should work
    const hsm_transition_t guard_continue_transitions[] = {
        {STATE_A, EVENT_NEXT, STATE_B, always_false_guard}, // This should fail
        {STATE_A, EVENT_NEXT, STATE_C, NULL},               // This should work
    };

    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, guard_continue_transitions, 2, STATE_A, &data));

    data.guard_called = false;

    // Should skip first transition due to guard, use second
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_NEXT));
    ASSERT_TRUE(data.guard_called);
    ASSERT_EQ(hsm.state, STATE_C); // Should transition to C, not B
}

/**
 * @test trigger_event_invalid_target_state
 * @brief Tests error handling when a transition specifies an invalid target state
 *
 * Purpose: Verifies that the HSM validates transition target states and safely
 * rejects transitions to non-existent states. This prevents array bounds violations
 * and maintains system stability when transition tables contain errors.
 *
 * Coverage: Tests target state validation and error return.
 */
UTEST(hsm, trigger_event_invalid_target_state) {
    hsm_t       hsm;
    test_data_t data = {0};

    // Create transition to invalid target state
    const hsm_transition_t bad_transitions[] = {
        {STATE_A, EVENT_NEXT, INVALID_STATE, NULL}, // Invalid target state
    };

    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, bad_transitions, 1, STATE_A, &data));

    // Should fail due to invalid target state
    ASSERT_FALSE(hsm_trigger_event(&hsm, EVENT_NEXT));
    ASSERT_EQ(hsm.state, STATE_A); // Should stay in original state
}

/**
 * @test trigger_event_no_matching_transition
 * @brief Tests behavior when no transition matches the current state and event
 *
 * Purpose: Verifies that when no transition is found in the table for the
 * current state/event combination, the HSM gracefully continues execution
 * without changing state. This is normal behavior for incomplete transition tables.
 *
 * Coverage: Tests the "no match found" return path.
 */
UTEST(hsm, trigger_event_no_matching_transition) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, minimal_state_machine, 2, minimal_transitions, 1, STATE_A, &data));

    // Trigger event that has no matching transition
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_RESET));
    ASSERT_EQ(hsm.state, STATE_A); // Should stay in same state
}

/**
 * @test trigger_event_with_null_action
 * @brief Tests HSM behavior when a state has no action function defined
 *
 * Purpose: Verifies that the HSM handles NULL action functions gracefully
 * without crashing. States may not always need action functions, so this
 * tests the optional nature of state actions.
 *
 * Coverage: Tests the NULL check when action is NULL.
 */
UTEST(hsm, trigger_event_with_null_action) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, action_optional_state_machine, 2, action_optional_transitions, 1, STATE_A, &data));

    // Should work even when action is NULL
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_NEXT));
    ASSERT_EQ(hsm.state, STATE_B);
}

/**
 * @test trigger_event_with_null_entry_exit
 * @brief Tests HSM behavior when states have no entry/exit functions defined
 *
 * Purpose: Verifies that the HSM handles NULL entry and exit functions gracefully
 * during state transitions. Entry/exit callbacks are optional, so this ensures
 * transitions work correctly when they're not needed.
 *
 * Coverage: Tests NULL checks for entry/exit callbacks.
 */
UTEST(hsm, trigger_event_with_null_entry_exit) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, minimal_state_machine, 2, minimal_transitions, 1, STATE_A, &data));

    // Should work even when entry/exit are NULL
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_NEXT));
    ASSERT_EQ(hsm.state, STATE_B);
}

/**
 * @test self_transition_with_guard
 * @brief Tests self-transitions (transitions from a state to itself)
 *
 * Purpose: Verifies that self-transitions work correctly and still call
 * both exit and entry callbacks even when transitioning to the same state.
 * This ensures consistent callback behavior and allows states to reset
 * themselves through self-transitions.
 *
 * Coverage: Tests self-transition logic and ensures exit/entry are called
 * even when prev_state == next_state.
 */
UTEST(hsm, self_transition_with_guard) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, test_transitions, 4, STATE_A, &data));

    data.guard_result = true;
    data.entry_called = false;
    data.exit_called  = false;

    // Trigger self-transition A -> A with guard
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_RESET));
    ASSERT_EQ(hsm.state, STATE_A);

    // Should still call exit and entry for self-transition
    ASSERT_TRUE(data.exit_called);
    ASSERT_EQ(data.last_exited_state, STATE_A);
    ASSERT_TRUE(data.entry_called);
    ASSERT_EQ(data.last_entered_state, STATE_A);
}

/**
 * @test hsm_init_macro_test
 * @brief Tests the HSM_INIT convenience macro for automatic array sizing
 *
 * Purpose: Verifies that the HSM_INIT macro correctly calculates array sizes
 * using sizeof() and properly initializes the HSM. This macro simplifies
 * HSM initialization by eliminating manual array size calculations.
 *
 * Coverage: Tests the macro expansion and underlying hsm_init() call with
 * automatically calculated array sizes.
 */
UTEST(hsm, hsm_init_macro_test) {
    hsm_t       hsm;
    test_data_t data = {0};

    // Test the HSM_INIT convenience macro
    ASSERT_TRUE(HSM_INIT(&hsm, test_state_machine, test_transitions, STATE_B, &data));
    ASSERT_EQ(hsm.state, STATE_B);
    ASSERT_TRUE(data.entry_called);
    ASSERT_EQ(data.last_entered_state, STATE_B);
}

/**
 * @test comprehensive_state_coverage
 * @brief Integration test that exercises all states and their action functions
 *
 * Purpose: Provides comprehensive coverage by visiting each state multiple times,
 * testing both state actions and transitions. This ensures all state-specific
 * logic works correctly and validates the complete state machine workflow
 * across all defined states.
 *
 * Coverage: Exercises action functions in all three states (A, B, C) and
 * validates consistent behavior throughout the state machine lifecycle.
 */
UTEST(hsm, comprehensive_state_coverage) {
    hsm_t       hsm;
    test_data_t data = {0};
    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, test_transitions, 4, STATE_A, &data));

    int expected_counter = 0;

    // Test action execution in each state
    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_INVALID)); // Stay in A, increment counter
    ASSERT_EQ(data.counter, ++expected_counter);
    ASSERT_EQ(hsm.state, STATE_A);

    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_NEXT)); // A -> B, increment counter
    ASSERT_EQ(data.counter, ++expected_counter);
    ASSERT_EQ(hsm.state, STATE_B);

    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_INVALID)); // Stay in B, increment counter
    ASSERT_EQ(data.counter, ++expected_counter);
    ASSERT_EQ(hsm.state, STATE_B);

    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_NEXT)); // B -> C, increment counter
    ASSERT_EQ(data.counter, ++expected_counter);
    ASSERT_EQ(hsm.state, STATE_C);

    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_INVALID)); // Stay in C, increment counter
    ASSERT_EQ(data.counter, ++expected_counter);
    ASSERT_EQ(hsm.state, STATE_C);

    ASSERT_TRUE(hsm_trigger_event(&hsm, EVENT_RESET)); // C -> A, increment counter
    ASSERT_EQ(data.counter, ++expected_counter);
    ASSERT_EQ(hsm.state, STATE_A);
}

//==============================================================================
// Test suite runner
//==============================================================================

UTEST_MAIN();
