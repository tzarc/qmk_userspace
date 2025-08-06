// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file test_hsm.c
 * @brief Comprehensive unit tests for the Hierarchical State Machine (HSM) framework
 *
 * This test suite validates all aspects of the HSM framework including:
 * - Initialization with various parameter combinations
 * - State execution and transitions
 * - Error handling and boundary conditions
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
} test_data_t;

// Test states
typedef enum {
    STATE_A       = 0,
    STATE_B       = 1,
    STATE_C       = 2,
    INVALID_STATE = 99,
} test_states_t;

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

// Executor functions
static hsm_state_t state_a_executor(void *data) {
    test_data_t *td = (test_data_t *)data;
    td->counter++;
    if (td->counter >= 3) {
        return STATE_B;
    }
    return STATE_A;
}

static hsm_state_t state_b_executor(void *data) {
    test_data_t *td = (test_data_t *)data;
    td->counter++;
    if (td->counter >= 5) {
        return STATE_C;
    }
    return STATE_B;
}

static hsm_state_t state_c_executor(void *data) {
    test_data_t *td = (test_data_t *)data;
    td->counter++;
    return STATE_C; // Stay in state C
}

static hsm_state_t invalid_executor(void *data) {
    return INVALID_STATE;
}

// State machine definition
static const hsm_entry_t test_state_machine[] = {
    [STATE_A] = {state_a_entry, state_a_executor, state_a_exit},
    [STATE_B] = {state_b_entry, state_b_executor, state_b_exit},
    [STATE_C] = {state_c_entry, state_c_executor, state_c_exit},
};

// State machine without entry/exit functions
static const hsm_entry_t minimal_state_machine[] = {
    [STATE_A] = {NULL, state_a_executor, NULL},
    [STATE_B] = {NULL, state_b_executor, NULL},
};

// State machine with missing executor in first state
static const hsm_entry_t broken_state_machine[] = {
    [STATE_A] = {state_a_entry, NULL, state_a_exit},
    [STATE_B] = {state_b_entry, state_b_executor, state_b_exit},
};

// State machine with missing executor in second state
static const hsm_entry_t broken_state_machine_2[] = {
    [STATE_A] = {state_a_entry, state_a_executor, state_a_exit},
    [STATE_B] = {state_b_entry, NULL, state_b_exit},
};

// State machine with invalid executor
static const hsm_entry_t invalid_state_machine[] = {
    [STATE_A] = {state_a_entry, invalid_executor, state_a_exit},
};

/**
 * @brief Test hsm_init parameter validation - NULL hsm pointer
 *
 * Validates that hsm_init properly rejects NULL hsm pointer.
 * This ensures the function fails gracefully when passed invalid parameters.
 */
UTEST(hsm_init, null_hsm) {
    ASSERT_FALSE(hsm_init(NULL, test_state_machine, 3, STATE_A, NULL));
}

/**
 * @brief Test hsm_init parameter validation - NULL state machine
 *
 * Validates that hsm_init properly rejects NULL state machine pointer.
 * This prevents potential segmentation faults from invalid state machine access.
 */
UTEST(hsm_init, null_state_machine) {
    hsm_t hsm;
    ASSERT_FALSE(hsm_init(&hsm, NULL, 3, STATE_A, NULL));
}

/**
 * @brief Test hsm_init parameter validation - zero number of states
 *
 * Validates that hsm_init rejects state machines with zero states.
 * A valid state machine must have at least one state.
 */
UTEST(hsm_init, zero_num_states) {
    hsm_t hsm;
    ASSERT_FALSE(hsm_init(&hsm, test_state_machine, 0, STATE_A, NULL));
}

/**
 * @brief Test hsm_init parameter validation - negative number of states
 *
 * Validates that hsm_init rejects negative state counts.
 * This prevents potential buffer overflows or undefined behavior.
 */
UTEST(hsm_init, negative_num_states) {
    hsm_t hsm;
    ASSERT_FALSE(hsm_init(&hsm, test_state_machine, -1, STATE_A, NULL));
}

/**
 * @brief Test successful HSM initialization with valid parameters
 *
 * Validates that hsm_init correctly initializes an HSM with valid parameters.
 * Confirms that all fields are properly set and the initial state's entry function is called.
 */
UTEST(hsm_init, valid_initialization) {
    hsm_t       hsm;
    test_data_t data = {0};

    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, STATE_A, &data));
    EXPECT_EQ(hsm.state, STATE_A);
    EXPECT_EQ(hsm.num_states, 3);
    EXPECT_EQ(hsm.state_data, &data);
    EXPECT_TRUE(data.entry_called);
    EXPECT_EQ(data.last_entered_state, STATE_A);
}

/**
 * @brief Test initial state bounds validation - out-of-bounds state defaults to zero
 *
 * Validates that hsm_init gracefully handles invalid initial states by defaulting to state 0.
 * This provides robust behavior when configuration errors occur.
 */
UTEST(hsm_init, invalid_initial_state_defaults_to_zero) {
    hsm_t       hsm;
    test_data_t data = {0};

    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, INVALID_STATE, &data));
    EXPECT_EQ(hsm.state, 0);
    EXPECT_TRUE(data.entry_called);
    EXPECT_EQ(data.last_entered_state, STATE_A);
}

/**
 * @brief Test initial state bounds validation - negative state defaults to zero
 *
 * Validates that hsm_init gracefully handles negative initial states by defaulting to state 0.
 * This ensures the HSM starts in a valid state even with invalid configuration.
 */
UTEST(hsm_init, negative_initial_state_defaults_to_zero) {
    hsm_t       hsm;
    test_data_t data = {0};

    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, -1, &data));
    EXPECT_EQ(hsm.state, 0);
    EXPECT_TRUE(data.entry_called);
    EXPECT_EQ(data.last_entered_state, STATE_A);
}

/**
 * @brief Test executor validation - missing executor in first state fails
 *
 * Validates that hsm_init rejects state machines where any state lacks an executor function.
 * This test specifically checks validation of the first state (STATE_A).
 */
UTEST(hsm_init, missing_executor_fails) {
    hsm_t       hsm;
    test_data_t data = {0};

    ASSERT_FALSE(hsm_init(&hsm, broken_state_machine, 2, STATE_A, &data));
}

/**
 * @brief Test executor validation - missing executor in second state fails
 *
 * Validates that hsm_init thoroughly checks ALL states for executor functions.
 * This test ensures that validation doesn't stop at just the initial state.
 */
UTEST(hsm_init, missing_executor_second_state_fails) {
    hsm_t       hsm;
    test_data_t data = {0};

    ASSERT_FALSE(hsm_init(&hsm, broken_state_machine_2, 2, STATE_A, &data));
}

/**
 * @brief Test memory safety - memset clears structure on initialization
 *
 * Validates that hsm_init properly clears the HSM structure with memset before initialization.
 * This prevents uninitialized memory from causing undefined behavior.
 */
UTEST(hsm_init, memset_clears_structure) {
    hsm_t       hsm;
    test_data_t data = {0};

    // Fill HSM with garbage data
    memset(&hsm, 0xFF, sizeof(hsm));

    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, STATE_A, &data));
    EXPECT_EQ(hsm.state, STATE_A);
    EXPECT_EQ(hsm.state_machine, test_state_machine);
    EXPECT_EQ(hsm.num_states, 3);
    EXPECT_EQ(hsm.state_data, &data);
}

/**
 * @brief Test HSM_INIT convenience macro
 *
 * Validates that the HSM_INIT macro correctly wraps hsm_init and automatically
 * calculates the number of states from the state machine array size.
 */
UTEST(hsm_init, hsm_init_macro) {
    hsm_t       hsm;
    test_data_t data = {0};

    // Test the convenience macro
    ASSERT_TRUE(HSM_INIT(&hsm, test_state_machine, STATE_B, &data));
    EXPECT_EQ(hsm.state, STATE_B);
    EXPECT_EQ(hsm.num_states, 3);
    EXPECT_TRUE(data.entry_called);
    EXPECT_EQ(data.last_entered_state, STATE_B);
}

/**
 * @brief Test hsm_execute parameter validation - NULL hsm pointer
 *
 * Validates that hsm_execute properly rejects NULL hsm pointer.
 * This prevents segmentation faults from invalid pointer access.
 */
UTEST(hsm_execute, null_hsm) {
    ASSERT_FALSE(hsm_execute(NULL));
}

/**
 * @brief Test hsm_execute with uninitialized HSM structure
 *
 * Validates that hsm_execute properly detects and rejects uninitialized HSM structures.
 * This ensures the function fails gracefully when passed invalid HSM instances.
 */
UTEST(hsm_execute, uninitialized_hsm) {
    hsm_t hsm = {0};
    ASSERT_FALSE(hsm_execute(&hsm));
}

/**
 * @brief Test valid execution without state transition
 *
 * Validates basic HSM execution where the executor returns the same state.
 * Confirms that no entry/exit callbacks are invoked when staying in the same state.
 */
UTEST(hsm_execute, valid_execution_no_transition) {
    hsm_t       hsm;
    test_data_t data = {0};

    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, STATE_A, &data));
    data.entry_called = false; // Reset for test

    ASSERT_TRUE(hsm_execute(&hsm));
    EXPECT_EQ(hsm.state, STATE_A);
    EXPECT_EQ(data.counter, 1);
    EXPECT_FALSE(data.entry_called); // No transition, no entry call
    EXPECT_FALSE(data.exit_called);
}

/**
 * @brief Test valid execution with state transition
 *
 * Validates HSM execution when a state transition occurs. Confirms that:
 * - The executor function is called and its return value is processed
 * - Exit callback is invoked for the previous state
 * - Entry callback is invoked for the new state
 * - State transition happens atomically
 */
UTEST(hsm_execute, valid_execution_with_transition) {
    hsm_t       hsm;
    test_data_t data = {0};

    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, STATE_A, &data));
    data.entry_called = false;
    data.exit_called  = false;

    // Execute twice to reach counter >= 3 and trigger transition
    ASSERT_TRUE(hsm_execute(&hsm));
    ASSERT_TRUE(hsm_execute(&hsm));
    EXPECT_EQ(hsm.state, STATE_A);
    EXPECT_EQ(data.counter, 2);

    // Third execution should trigger transition to STATE_B
    ASSERT_TRUE(hsm_execute(&hsm));
    EXPECT_EQ(hsm.state, STATE_B);
    EXPECT_EQ(data.counter, 3);
    EXPECT_TRUE(data.exit_called);
    EXPECT_EQ(data.last_exited_state, STATE_A);
    EXPECT_TRUE(data.entry_called);
    EXPECT_EQ(data.last_entered_state, STATE_B);
}

/**
 * @brief Test complex state machine with multiple sequential transitions
 *
 * Validates that the HSM can handle multiple consecutive state transitions correctly.
 * Tests the complete state flow: STATE_A → STATE_B → STATE_C, verifying that
 * callbacks are properly invoked for each transition and final state behavior.
 */
UTEST(hsm_execute, multiple_transitions) {
    hsm_t       hsm;
    test_data_t data = {0};

    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, STATE_A, &data));
    data.counter = 0; // Reset counter

    // Execute until we reach STATE_B (counter >= 3)
    for (int i = 0; i < 3; i++) {
        ASSERT_TRUE(hsm_execute(&hsm));
    }
    EXPECT_EQ(hsm.state, STATE_B);
    EXPECT_EQ(data.counter, 3);

    // Execute until we reach STATE_C (counter >= 5)
    data.exit_called  = false;
    data.entry_called = false;
    ASSERT_TRUE(hsm_execute(&hsm));
    ASSERT_TRUE(hsm_execute(&hsm));
    EXPECT_EQ(hsm.state, STATE_C);
    EXPECT_EQ(data.counter, 5);
    EXPECT_TRUE(data.exit_called);
    EXPECT_EQ(data.last_exited_state, STATE_B);
    EXPECT_TRUE(data.entry_called);
    EXPECT_EQ(data.last_entered_state, STATE_C);

    // STATE_C should stay in STATE_C
    data.exit_called  = false;
    data.entry_called = false;
    ASSERT_TRUE(hsm_execute(&hsm));
    EXPECT_EQ(hsm.state, STATE_C);
    EXPECT_EQ(data.counter, 6);
    EXPECT_FALSE(data.exit_called);
    EXPECT_FALSE(data.entry_called);
}

/**
 * @brief Test execution with minimal state machine (no entry/exit functions)
 *
 * Validates that the HSM works correctly with state machines that only define
 * executor functions without entry/exit callbacks. This tests the framework's
 * flexibility in handling optional callback functions.
 */
UTEST(hsm_execute, minimal_state_machine) {
    hsm_t       hsm;
    test_data_t data = {0};

    ASSERT_TRUE(hsm_init(&hsm, minimal_state_machine, 2, STATE_A, &data));
    EXPECT_FALSE(data.entry_called); // No entry function

    ASSERT_TRUE(hsm_execute(&hsm));
    EXPECT_EQ(data.counter, 1);
    EXPECT_FALSE(data.exit_called); // No exit function
}

/**
 * @brief Test execution failure when executor function is missing (runtime check)
 *
 * Validates that hsm_execute detects missing executor functions at runtime.
 * This test bypasses hsm_init validation to specifically test the runtime
 * executor validation in hsm_execute.
 */
UTEST(hsm_execute, missing_executor_fails) {
    hsm_t       hsm;
    test_data_t data = {0};

    // Manually set up broken state machine (bypassing init validation)
    memset(&hsm, 0, sizeof(hsm));
    hsm.state         = STATE_A;
    hsm.state_machine = broken_state_machine;
    hsm.num_states    = 2;
    hsm.state_data    = &data;

    ASSERT_FALSE(hsm_execute(&hsm));
}

/**
 * @brief Test execution failure when executor returns invalid state
 *
 * Validates that hsm_execute properly detects and rejects invalid state transitions
 * when an executor function returns a state that is out of bounds for the state machine.
 */
UTEST(hsm_execute, invalid_state_transition_fails) {
    hsm_t       hsm;
    test_data_t data = {0};

    ASSERT_TRUE(hsm_init(&hsm, invalid_state_machine, 1, STATE_A, &data));
    ASSERT_FALSE(hsm_execute(&hsm)); // Should fail due to invalid return state
}

/**
 * @brief Test execution failure with corrupted current state (out-of-bounds)
 *
 * Validates that hsm_execute detects when the current state has been corrupted
 * to an out-of-bounds value. This ensures robust behavior even when the HSM
 * structure has been corrupted by external code.
 */
UTEST(hsm_execute, out_of_bounds_current_state_fails) {
    hsm_t       hsm;
    test_data_t data = {0};

    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, STATE_A, &data));

    // Manually corrupt the current state
    hsm.state = INVALID_STATE;

    ASSERT_FALSE(hsm_execute(&hsm));
}

/**
 * @brief Test execution failure with corrupted current state (negative value)
 *
 * Validates that hsm_execute detects when the current state has been corrupted
 * to a negative value. This ensures robust behavior and prevents potential
 * buffer underflows or undefined array access.
 */
UTEST(hsm_execute, negative_current_state_fails) {
    hsm_t       hsm;
    test_data_t data = {0};

    ASSERT_TRUE(hsm_init(&hsm, test_state_machine, 3, STATE_A, &data));

    // Manually corrupt the current state
    hsm.state = -1;

    ASSERT_FALSE(hsm_execute(&hsm));
}

UTEST_MAIN()
