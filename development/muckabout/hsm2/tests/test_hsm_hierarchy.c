// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file test_hsm_hierarchy.c
 * @brief Hierarchical State Machine test demonstrating nested state machines
 *
 * This test suite demonstrates how to use the HSM framework to create
 * hierarchical state machines by composing multiple HSM instances together.
 * It implements a smart home automation system with multiple subsystems:
 * - Security system (armed/disarmed with alarm states)
 * - Climate control (heating/cooling/off)
 * - Lighting system (day/night/motion modes)
 * - Main controller that coordinates all subsystems
 *
 * This showcases real-world usage patterns for complex state machine hierarchies.
 */

#include "utest.h"
#include "../src/hsm.h"
#include <string.h>

//==============================================================================
// Smart Home System Data Structures
//==============================================================================

typedef struct {
    int  temperature;
    bool motion_detected;
    bool door_opened;
    bool user_present;
    int  time_of_day; // 0-23 hours
    bool alarm_triggered;
    int  light_level; // 0-100%
} sensor_data_t;

typedef struct {
    hsm_t         security_hsm;
    hsm_t         climate_hsm;
    hsm_t         lighting_hsm;
    hsm_t         main_hsm;
    sensor_data_t sensors;
    bool          system_active;
    int           transitions_count;
} smart_home_t;

//==============================================================================
// Security System States & Events
//==============================================================================

typedef enum {
    SEC_DISARMED   = 0,
    SEC_ARMED_HOME = 1,
    SEC_ARMED_AWAY = 2,
    SEC_ALARM      = 3,
} security_states_t;

typedef enum {
    SEC_EVT_ARM_HOME  = 0,
    SEC_EVT_ARM_AWAY  = 1,
    SEC_EVT_DISARM    = 2,
    SEC_EVT_INTRUSION = 3,
    SEC_EVT_TIMEOUT   = 4,
} security_events_t;

// Security system callbacks
static void sec_disarmed_entry(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    printf("Security: System disarmed\n");
}

static void sec_armed_home_entry(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    printf("Security: Armed in home mode\n");
}

static void sec_armed_away_entry(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    printf("Security: Armed in away mode\n");
}

static void sec_alarm_entry(void *data) {
    smart_home_t *home            = (smart_home_t *)data;
    home->sensors.alarm_triggered = true;
    printf("Security: ALARM! Intrusion detected!\n");
}

static void sec_alarm_action(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    // Alarm action could trigger other systems
    home->transitions_count++;
}

// Security guard conditions
static bool sec_can_arm_away(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    return !home->sensors.user_present; // Can only arm away if no one is home
}

static bool sec_intrusion_detected(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    return home->sensors.door_opened || home->sensors.motion_detected;
}

//==============================================================================
// Climate Control States & Events
//==============================================================================

typedef enum {
    CLIMATE_OFF     = 0,
    CLIMATE_HEATING = 1,
    CLIMATE_COOLING = 2,
    CLIMATE_AUTO    = 3,
} climate_states_t;

typedef enum {
    CLIMATE_EVT_TURN_ON    = 0,
    CLIMATE_EVT_TURN_OFF   = 1,
    CLIMATE_EVT_SET_HEAT   = 2,
    CLIMATE_EVT_SET_COOL   = 3,
    CLIMATE_EVT_SET_AUTO   = 4,
    CLIMATE_EVT_TEMP_CHECK = 5,
} climate_events_t;

static void climate_off_entry(void *data) {
    printf("Climate: System off\n");
}

static void climate_heating_entry(void *data) {
    printf("Climate: Heating mode activated\n");
}

static void climate_cooling_entry(void *data) {
    printf("Climate: Cooling mode activated\n");
}

static void climate_auto_entry(void *data) {
    printf("Climate: Auto mode - adjusting based on temperature\n");
}

static void climate_auto_action(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    // Auto mode logic - could trigger heating/cooling based on temperature
    if (home->sensors.temperature < 68) {
        printf("Climate: Auto mode - temperature low (%d°F), would heat\n", home->sensors.temperature);
    } else if (home->sensors.temperature > 75) {
        printf("Climate: Auto mode - temperature high (%d°F), would cool\n", home->sensors.temperature);
    }
}

static bool climate_needs_heating(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    return home->sensors.temperature < 68;
}

static bool climate_needs_cooling(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    return home->sensors.temperature > 75;
}

//==============================================================================
// Lighting System States & Events
//==============================================================================

typedef enum {
    LIGHT_OFF         = 0,
    LIGHT_DAY_MODE    = 1,
    LIGHT_NIGHT_MODE  = 2,
    LIGHT_MOTION_MODE = 3,
} lighting_states_t;

typedef enum {
    LIGHT_EVT_TURN_ON         = 0,
    LIGHT_EVT_TURN_OFF        = 1,
    LIGHT_EVT_DAY_MODE        = 2,
    LIGHT_EVT_NIGHT_MODE      = 3,
    LIGHT_EVT_MOTION_MODE     = 4,
    LIGHT_EVT_MOTION_DETECTED = 5,
    LIGHT_EVT_TIME_UPDATE     = 6,
} lighting_events_t;

static void light_off_entry(void *data) {
    smart_home_t *home        = (smart_home_t *)data;
    home->sensors.light_level = 0;
    printf("Lighting: All lights off\n");
}

static void light_day_entry(void *data) {
    smart_home_t *home        = (smart_home_t *)data;
    home->sensors.light_level = 60;
    printf("Lighting: Day mode - moderate lighting (%d%%)\n", home->sensors.light_level);
}

static void light_night_entry(void *data) {
    smart_home_t *home        = (smart_home_t *)data;
    home->sensors.light_level = 20;
    printf("Lighting: Night mode - dim lighting (%d%%)\n", home->sensors.light_level);
}

static void light_motion_entry(void *data) {
    smart_home_t *home        = (smart_home_t *)data;
    home->sensors.light_level = 80;
    printf("Lighting: Motion mode - bright lighting (%d%%)\n", home->sensors.light_level);
}

static void light_motion_action(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    // Motion mode could automatically adjust based on time
    if (home->sensors.time_of_day >= 22 || home->sensors.time_of_day <= 6) {
        home->sensors.light_level = 40; // Dimmer at night
        printf("Lighting: Motion mode - dimmed for night time (%d%%)\n", home->sensors.light_level);
    }
}

static bool is_daytime(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    return home->sensors.time_of_day >= 6 && home->sensors.time_of_day < 22;
}

static bool is_nighttime(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    return home->sensors.time_of_day >= 22 || home->sensors.time_of_day < 6;
}

//==============================================================================
// Main System Controller States & Events
//==============================================================================

typedef enum {
    MAIN_STARTUP   = 0,
    MAIN_NORMAL    = 1,
    MAIN_AWAY      = 2,
    MAIN_EMERGENCY = 3,
    MAIN_SHUTDOWN  = 4,
} main_states_t;

typedef enum {
    MAIN_EVT_SYSTEM_READY    = 0,
    MAIN_EVT_USER_LEAVES     = 1,
    MAIN_EVT_USER_RETURNS    = 2,
    MAIN_EVT_EMERGENCY       = 3,
    MAIN_EVT_SHUTDOWN        = 4,
    MAIN_EVT_PERIODIC_UPDATE = 5,
} main_events_t;

static void main_startup_entry(void *data) {
    smart_home_t *home  = (smart_home_t *)data;
    home->system_active = true;
    printf("Main: System starting up - initializing all subsystems\n");
}

static void main_normal_entry(void *data) {
    printf("Main: Normal operation mode\n");
}

static void main_normal_action(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    // Coordinate subsystems during normal operation
    home->transitions_count++;

    // Example coordination: trigger climate check
    hsm_trigger_event(&home->climate_hsm, CLIMATE_EVT_TEMP_CHECK);

    // Example coordination: update lighting based on time
    hsm_trigger_event(&home->lighting_hsm, LIGHT_EVT_TIME_UPDATE);
}

static void main_away_entry(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    printf("Main: Away mode - enabling energy saving\n");

    // Coordinate subsystems for away mode
    hsm_trigger_event(&home->security_hsm, SEC_EVT_ARM_AWAY);
    hsm_trigger_event(&home->climate_hsm, CLIMATE_EVT_SET_AUTO);
    hsm_trigger_event(&home->lighting_hsm, LIGHT_EVT_TURN_OFF);
}

static void main_emergency_entry(void *data) {
    smart_home_t *home = (smart_home_t *)data;
    printf("Main: EMERGENCY MODE - all systems on high alert\n");

    // Emergency coordination
    hsm_trigger_event(&home->lighting_hsm, LIGHT_EVT_MOTION_MODE); // Full brightness
}

static void main_shutdown_entry(void *data) {
    smart_home_t *home  = (smart_home_t *)data;
    home->system_active = false;
    printf("Main: System shutting down\n");

    // Shutdown all subsystems
    hsm_trigger_event(&home->security_hsm, SEC_EVT_DISARM);
    hsm_trigger_event(&home->climate_hsm, CLIMATE_EVT_TURN_OFF);
    hsm_trigger_event(&home->lighting_hsm, LIGHT_EVT_TURN_OFF);
}

//==============================================================================
// State Machine Definitions
//==============================================================================

static const hsm_entry_t security_states[] = {
    [SEC_DISARMED]   = {sec_disarmed_entry, NULL, NULL},
    [SEC_ARMED_HOME] = {sec_armed_home_entry, NULL, NULL},
    [SEC_ARMED_AWAY] = {sec_armed_away_entry, NULL, NULL},
    [SEC_ALARM]      = {sec_alarm_entry, sec_alarm_action, NULL},
};

static const hsm_transition_t security_transitions[] = {
    {SEC_DISARMED, SEC_EVT_ARM_HOME, SEC_ARMED_HOME, NULL}, {SEC_DISARMED, SEC_EVT_ARM_AWAY, SEC_ARMED_AWAY, sec_can_arm_away}, {SEC_ARMED_HOME, SEC_EVT_DISARM, SEC_DISARMED, NULL}, {SEC_ARMED_HOME, SEC_EVT_INTRUSION, SEC_ALARM, sec_intrusion_detected}, {SEC_ARMED_AWAY, SEC_EVT_DISARM, SEC_DISARMED, NULL}, {SEC_ARMED_AWAY, SEC_EVT_INTRUSION, SEC_ALARM, sec_intrusion_detected}, {SEC_ALARM, SEC_EVT_DISARM, SEC_DISARMED, NULL}, {SEC_ALARM, SEC_EVT_TIMEOUT, SEC_ARMED_AWAY, NULL},
};

static const hsm_entry_t climate_states[] = {
    [CLIMATE_OFF]     = {climate_off_entry, NULL, NULL}, //
    [CLIMATE_HEATING] = {climate_heating_entry, NULL, NULL},
    [CLIMATE_COOLING] = {climate_cooling_entry, NULL, NULL},
    [CLIMATE_AUTO]    = {climate_auto_entry, climate_auto_action, NULL},
};

static const hsm_transition_t climate_transitions[] = {
    {CLIMATE_OFF, CLIMATE_EVT_TURN_ON, CLIMATE_AUTO, NULL}, {CLIMATE_OFF, CLIMATE_EVT_SET_HEAT, CLIMATE_HEATING, NULL}, {CLIMATE_OFF, CLIMATE_EVT_SET_COOL, CLIMATE_COOLING, NULL}, {CLIMATE_HEATING, CLIMATE_EVT_TURN_OFF, CLIMATE_OFF, NULL}, {CLIMATE_HEATING, CLIMATE_EVT_SET_AUTO, CLIMATE_AUTO, NULL}, {CLIMATE_COOLING, CLIMATE_EVT_TURN_OFF, CLIMATE_OFF, NULL}, {CLIMATE_COOLING, CLIMATE_EVT_SET_AUTO, CLIMATE_AUTO, NULL}, {CLIMATE_AUTO, CLIMATE_EVT_TURN_OFF, CLIMATE_OFF, NULL}, {CLIMATE_AUTO, CLIMATE_EVT_SET_HEAT, CLIMATE_HEATING, climate_needs_heating}, {CLIMATE_AUTO, CLIMATE_EVT_SET_COOL, CLIMATE_COOLING, climate_needs_cooling},
};

static const hsm_entry_t lighting_states[] = {
    [LIGHT_OFF]         = {light_off_entry, NULL, NULL}, //
    [LIGHT_DAY_MODE]    = {light_day_entry, NULL, NULL},
    [LIGHT_NIGHT_MODE]  = {light_night_entry, NULL, NULL},
    [LIGHT_MOTION_MODE] = {light_motion_entry, light_motion_action, NULL},
};

static const hsm_transition_t lighting_transitions[] = {
    {LIGHT_OFF, LIGHT_EVT_TURN_ON, LIGHT_DAY_MODE, is_daytime},
    {LIGHT_OFF, LIGHT_EVT_TURN_ON, LIGHT_NIGHT_MODE, is_nighttime},
    {LIGHT_OFF, LIGHT_EVT_DAY_MODE, LIGHT_DAY_MODE, NULL},
    {LIGHT_OFF, LIGHT_EVT_NIGHT_MODE, LIGHT_NIGHT_MODE, NULL},
    {LIGHT_OFF, LIGHT_EVT_MOTION_MODE, LIGHT_MOTION_MODE, NULL},
    {LIGHT_DAY_MODE, LIGHT_EVT_TURN_OFF, LIGHT_OFF, NULL},
    {LIGHT_DAY_MODE, LIGHT_EVT_NIGHT_MODE, LIGHT_NIGHT_MODE, NULL},
    {LIGHT_DAY_MODE, LIGHT_EVT_MOTION_DETECTED, LIGHT_MOTION_MODE, NULL},
    {LIGHT_DAY_MODE, LIGHT_EVT_MOTION_MODE, LIGHT_MOTION_MODE, NULL}, // Emergency override
    {LIGHT_NIGHT_MODE, LIGHT_EVT_TURN_OFF, LIGHT_OFF, NULL},
    {LIGHT_NIGHT_MODE, LIGHT_EVT_DAY_MODE, LIGHT_DAY_MODE, NULL},
    {LIGHT_NIGHT_MODE, LIGHT_EVT_MOTION_DETECTED, LIGHT_MOTION_MODE, NULL},
    {LIGHT_NIGHT_MODE, LIGHT_EVT_MOTION_MODE, LIGHT_MOTION_MODE, NULL}, // Emergency override
    {LIGHT_MOTION_MODE, LIGHT_EVT_TURN_OFF, LIGHT_OFF, NULL},
    {LIGHT_MOTION_MODE, LIGHT_EVT_DAY_MODE, LIGHT_DAY_MODE, is_daytime},
    {LIGHT_MOTION_MODE, LIGHT_EVT_NIGHT_MODE, LIGHT_NIGHT_MODE, is_nighttime},
};

static const hsm_entry_t main_states[] = {
    [MAIN_STARTUP]   = {main_startup_entry, NULL, NULL}, //
    [MAIN_NORMAL]    = {main_normal_entry, main_normal_action, NULL},
    [MAIN_AWAY]      = {main_away_entry, NULL, NULL},
    [MAIN_EMERGENCY] = {main_emergency_entry, NULL, NULL},
    [MAIN_SHUTDOWN]  = {main_shutdown_entry, NULL, NULL},
};

static const hsm_transition_t main_transitions[] = {
    {MAIN_STARTUP, MAIN_EVT_SYSTEM_READY, MAIN_NORMAL, NULL}, {MAIN_NORMAL, MAIN_EVT_USER_LEAVES, MAIN_AWAY, NULL}, {MAIN_NORMAL, MAIN_EVT_EMERGENCY, MAIN_EMERGENCY, NULL}, {MAIN_NORMAL, MAIN_EVT_SHUTDOWN, MAIN_SHUTDOWN, NULL}, {MAIN_AWAY, MAIN_EVT_USER_RETURNS, MAIN_NORMAL, NULL}, {MAIN_AWAY, MAIN_EVT_EMERGENCY, MAIN_EMERGENCY, NULL}, {MAIN_AWAY, MAIN_EVT_SHUTDOWN, MAIN_SHUTDOWN, NULL}, {MAIN_EMERGENCY, MAIN_EVT_USER_RETURNS, MAIN_NORMAL, NULL}, {MAIN_EMERGENCY, MAIN_EVT_SHUTDOWN, MAIN_SHUTDOWN, NULL},
};

//==============================================================================
// Test Functions
//==============================================================================

/**
 * @test hsm_hierarchy_initialization
 * @brief Tests initialization of multiple hierarchical state machines
 *
 * Purpose: Verifies that multiple HSM instances can be properly initialized
 * and composed together to create a hierarchical system. This tests the
 * framework's ability to support complex, multi-layered state machine designs.
 *
 * Coverage: Tests multiple HSM initialization and initial state coordination.
 */
UTEST(hsm_hierarchy, initialization) {
    smart_home_t home = {0};

    // Initialize sensor data
    home.sensors.temperature  = 72;
    home.sensors.time_of_day  = 14; // 2 PM
    home.sensors.user_present = true;

    // Initialize all subsystem state machines
    ASSERT_TRUE(HSM_INIT(&home.security_hsm, security_states, security_transitions, SEC_DISARMED, &home));
    ASSERT_TRUE(HSM_INIT(&home.climate_hsm, climate_states, climate_transitions, CLIMATE_OFF, &home));
    ASSERT_TRUE(HSM_INIT(&home.lighting_hsm, lighting_states, lighting_transitions, LIGHT_OFF, &home));
    ASSERT_TRUE(HSM_INIT(&home.main_hsm, main_states, main_transitions, MAIN_STARTUP, &home));

    // Verify initial states
    ASSERT_EQ(home.security_hsm.state, SEC_DISARMED);
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_OFF);
    ASSERT_EQ(home.lighting_hsm.state, LIGHT_OFF);
    ASSERT_EQ(home.main_hsm.state, MAIN_STARTUP);
    ASSERT_TRUE(home.system_active);
}

/**
 * @test hsm_hierarchy_coordination
 * @brief Tests coordination between hierarchical state machines
 *
 * Purpose: Verifies that the main controller can coordinate actions across
 * multiple subsystems by triggering events in child state machines. This
 * demonstrates the hierarchical coordination pattern.
 *
 * Coverage: Tests cross-HSM event triggering and state coordination.
 */
UTEST(hsm_hierarchy, coordination) {
    smart_home_t home         = {0};
    home.sensors.temperature  = 72;
    home.sensors.time_of_day  = 20; // 8 PM
    home.sensors.user_present = true;

    // Initialize all state machines
    ASSERT_TRUE(HSM_INIT(&home.security_hsm, security_states, security_transitions, SEC_DISARMED, &home));
    ASSERT_TRUE(HSM_INIT(&home.climate_hsm, climate_states, climate_transitions, CLIMATE_OFF, &home));
    ASSERT_TRUE(HSM_INIT(&home.lighting_hsm, lighting_states, lighting_transitions, LIGHT_OFF, &home));
    ASSERT_TRUE(HSM_INIT(&home.main_hsm, main_states, main_transitions, MAIN_STARTUP, &home));

    // Start the system
    ASSERT_TRUE(hsm_trigger_event(&home.main_hsm, MAIN_EVT_SYSTEM_READY));
    ASSERT_EQ(home.main_hsm.state, MAIN_NORMAL);

    // Test coordination when user leaves
    home.sensors.user_present = false;
    ASSERT_TRUE(hsm_trigger_event(&home.main_hsm, MAIN_EVT_USER_LEAVES));
    ASSERT_EQ(home.main_hsm.state, MAIN_AWAY);

    // Verify subsystems were coordinated (away mode should have triggered subsystem changes)
    ASSERT_EQ(home.security_hsm.state, SEC_ARMED_AWAY);
    // Note: Climate system stays in OFF state since it wasn't turned on initially
    // The CLIMATE_EVT_SET_AUTO event doesn't turn on an OFF system, it just changes mode
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_OFF);
    ASSERT_EQ(home.lighting_hsm.state, LIGHT_OFF);
}

/**
 * @test hsm_hierarchy_security_system
 * @brief Tests the security subsystem state transitions and guard conditions
 *
 * Purpose: Validates the security system's state machine behavior including
 * guard conditions that prevent invalid operations (like arming away when
 * users are present) and intrusion detection logic.
 *
 * Coverage: Tests security system states, transitions, and guard conditions.
 */
UTEST(hsm_hierarchy, security_system) {
    smart_home_t home         = {0};
    home.sensors.user_present = true;

    ASSERT_TRUE(HSM_INIT(&home.security_hsm, security_states, security_transitions, SEC_DISARMED, &home));
    ASSERT_EQ(home.security_hsm.state, SEC_DISARMED);

    // Should be able to arm home mode with user present
    ASSERT_TRUE(hsm_trigger_event(&home.security_hsm, SEC_EVT_ARM_HOME));
    ASSERT_EQ(home.security_hsm.state, SEC_ARMED_HOME);

    // Should NOT be able to arm away mode with user present (guard should fail)
    ASSERT_TRUE(hsm_trigger_event(&home.security_hsm, SEC_EVT_ARM_AWAY));
    ASSERT_EQ(home.security_hsm.state, SEC_ARMED_HOME); // Should remain in home mode

    // User leaves, now can arm away
    home.sensors.user_present = false;
    ASSERT_TRUE(hsm_trigger_event(&home.security_hsm, SEC_EVT_DISARM));
    ASSERT_EQ(home.security_hsm.state, SEC_DISARMED);

    ASSERT_TRUE(hsm_trigger_event(&home.security_hsm, SEC_EVT_ARM_AWAY));
    ASSERT_EQ(home.security_hsm.state, SEC_ARMED_AWAY);

    // Test intrusion detection
    home.sensors.door_opened = true;
    ASSERT_TRUE(hsm_trigger_event(&home.security_hsm, SEC_EVT_INTRUSION));
    ASSERT_EQ(home.security_hsm.state, SEC_ALARM);
    ASSERT_TRUE(home.sensors.alarm_triggered);
}

/**
 * @test hsm_hierarchy_climate_control
 * @brief Tests the climate control subsystem with temperature-based logic
 *
 * Purpose: Validates the climate control system's automatic temperature
 * management including guard conditions that trigger heating/cooling based
 * on current temperature readings.
 *
 * Coverage: Tests climate system states, auto mode logic, and temperature guards.
 */
UTEST(hsm_hierarchy, climate_control) {
    smart_home_t home        = {0};
    home.sensors.temperature = 72; // Comfortable temperature

    ASSERT_TRUE(HSM_INIT(&home.climate_hsm, climate_states, climate_transitions, CLIMATE_OFF, &home));

    // Turn on climate system (should go to auto mode)
    ASSERT_TRUE(hsm_trigger_event(&home.climate_hsm, CLIMATE_EVT_TURN_ON));
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_AUTO);

    // Test heating logic - set cold temperature
    home.sensors.temperature = 65; // Too cold
    ASSERT_TRUE(hsm_trigger_event(&home.climate_hsm, CLIMATE_EVT_SET_HEAT));
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_HEATING); // Guard should pass

    // Test cooling logic - set hot temperature
    home.sensors.temperature = 80; // Too hot
    ASSERT_TRUE(hsm_trigger_event(&home.climate_hsm, CLIMATE_EVT_SET_AUTO));
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_AUTO);

    ASSERT_TRUE(hsm_trigger_event(&home.climate_hsm, CLIMATE_EVT_SET_COOL));
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_COOLING); // Guard should pass

    // Test that auto mode doesn't switch when temperature is comfortable
    home.sensors.temperature = 72; // Comfortable
    ASSERT_TRUE(hsm_trigger_event(&home.climate_hsm, CLIMATE_EVT_SET_AUTO));
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_AUTO);

    // These should fail due to guard conditions
    ASSERT_TRUE(hsm_trigger_event(&home.climate_hsm, CLIMATE_EVT_SET_HEAT));
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_AUTO); // Should stay in auto

    ASSERT_TRUE(hsm_trigger_event(&home.climate_hsm, CLIMATE_EVT_SET_COOL));
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_AUTO); // Should stay in auto
}

/**
 * @test hsm_hierarchy_lighting_system
 * @brief Tests the lighting subsystem with time-based and motion-based logic
 *
 * Purpose: Validates the lighting system's automatic mode selection based
 * on time of day and motion detection, including guard conditions for
 * day/night transitions.
 *
 * Coverage: Tests lighting states, time-based guards, and motion detection.
 */
UTEST(hsm_hierarchy, lighting_system) {
    smart_home_t home = {0};

    ASSERT_TRUE(HSM_INIT(&home.lighting_hsm, lighting_states, lighting_transitions, LIGHT_OFF, &home));

    // Test daytime activation
    home.sensors.time_of_day = 14; // 2 PM
    ASSERT_TRUE(hsm_trigger_event(&home.lighting_hsm, LIGHT_EVT_TURN_ON));
    ASSERT_EQ(home.lighting_hsm.state, LIGHT_DAY_MODE);
    ASSERT_EQ(home.sensors.light_level, 60);

    // Test nighttime transition
    home.sensors.time_of_day = 23; // 11 PM
    ASSERT_TRUE(hsm_trigger_event(&home.lighting_hsm, LIGHT_EVT_NIGHT_MODE));
    ASSERT_EQ(home.lighting_hsm.state, LIGHT_NIGHT_MODE);
    ASSERT_EQ(home.sensors.light_level, 20);

    // Test motion detection
    home.sensors.motion_detected = true;
    ASSERT_TRUE(hsm_trigger_event(&home.lighting_hsm, LIGHT_EVT_MOTION_DETECTED));
    ASSERT_EQ(home.lighting_hsm.state, LIGHT_MOTION_MODE);
    ASSERT_EQ(home.sensors.light_level, 80);

    // Test motion mode action (should dim for nighttime)
    ASSERT_TRUE(hsm_trigger_event(&home.lighting_hsm, LIGHT_EVT_TIME_UPDATE));
    ASSERT_EQ(home.sensors.light_level, 40); // Should be dimmed for night

    // Test time-based transitions from motion mode
    home.sensors.time_of_day = 10; // 10 AM
    ASSERT_TRUE(hsm_trigger_event(&home.lighting_hsm, LIGHT_EVT_DAY_MODE));
    ASSERT_EQ(home.lighting_hsm.state, LIGHT_DAY_MODE);
}

/**
 * @test hsm_hierarchy_emergency_scenario
 * @brief Tests emergency coordination across all subsystems
 *
 * Purpose: Validates that the main controller can coordinate an emergency
 * response across all subsystems, demonstrating complex hierarchical
 * state machine coordination in critical scenarios.
 *
 * Coverage: Tests emergency state coordination and multi-HSM orchestration.
 */
UTEST(hsm_hierarchy, emergency_scenario) {
    smart_home_t home         = {0};
    home.sensors.temperature  = 72;
    home.sensors.time_of_day  = 20;
    home.sensors.user_present = true;

    // Initialize all systems in normal operation
    ASSERT_TRUE(HSM_INIT(&home.security_hsm, security_states, security_transitions, SEC_DISARMED, &home));
    ASSERT_TRUE(HSM_INIT(&home.climate_hsm, climate_states, climate_transitions, CLIMATE_AUTO, &home));
    ASSERT_TRUE(HSM_INIT(&home.lighting_hsm, lighting_states, lighting_transitions, LIGHT_DAY_MODE, &home));
    ASSERT_TRUE(HSM_INIT(&home.main_hsm, main_states, main_transitions, MAIN_NORMAL, &home));

    // Trigger emergency
    ASSERT_TRUE(hsm_trigger_event(&home.main_hsm, MAIN_EVT_EMERGENCY));
    ASSERT_EQ(home.main_hsm.state, MAIN_EMERGENCY);

    // Verify emergency coordination - lighting should have been set to motion mode
    // Note: The emergency entry function triggers the lighting event, but the lighting
    // system needs to be able to transition from its current state to motion mode
    ASSERT_EQ(home.lighting_hsm.state, LIGHT_MOTION_MODE); // Full brightness for emergency
    ASSERT_EQ(home.sensors.light_level, 80);

    // Test recovery from emergency
    ASSERT_TRUE(hsm_trigger_event(&home.main_hsm, MAIN_EVT_USER_RETURNS));
    ASSERT_EQ(home.main_hsm.state, MAIN_NORMAL);

    // Test system shutdown coordination
    ASSERT_TRUE(hsm_trigger_event(&home.main_hsm, MAIN_EVT_SHUTDOWN));
    ASSERT_EQ(home.main_hsm.state, MAIN_SHUTDOWN);
    ASSERT_FALSE(home.system_active);

    // Verify all subsystems were coordinated for shutdown
    ASSERT_EQ(home.security_hsm.state, SEC_DISARMED);
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_OFF);
    ASSERT_EQ(home.lighting_hsm.state, LIGHT_OFF);
    ASSERT_EQ(home.sensors.light_level, 0);
}

/**
 * @test hsm_hierarchy_complex_workflow
 * @brief Tests a complete workflow with multiple state transitions across subsystems
 *
 * Purpose: Validates the entire hierarchical system through a realistic
 * day-in-the-life scenario with multiple coordinated state changes across
 * all subsystems. This provides integration testing of the complete hierarchy.
 *
 * Coverage: Tests complete workflow integration and subsystem coordination.
 */
UTEST(hsm_hierarchy, complex_workflow) {
    smart_home_t home         = {0};
    home.sensors.temperature  = 72;
    home.sensors.time_of_day  = 8; // 8 AM
    home.sensors.user_present = true;

    // Initialize system
    ASSERT_TRUE(HSM_INIT(&home.security_hsm, security_states, security_transitions, SEC_DISARMED, &home));
    ASSERT_TRUE(HSM_INIT(&home.climate_hsm, climate_states, climate_transitions, CLIMATE_OFF, &home));
    ASSERT_TRUE(HSM_INIT(&home.lighting_hsm, lighting_states, lighting_transitions, LIGHT_OFF, &home));
    ASSERT_TRUE(HSM_INIT(&home.main_hsm, main_states, main_transitions, MAIN_STARTUP, &home));

    // 1. Morning startup
    ASSERT_TRUE(hsm_trigger_event(&home.main_hsm, MAIN_EVT_SYSTEM_READY));
    ASSERT_EQ(home.main_hsm.state, MAIN_NORMAL);

    // 2. Turn on systems for the day
    ASSERT_TRUE(hsm_trigger_event(&home.climate_hsm, CLIMATE_EVT_TURN_ON));
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_AUTO);

    ASSERT_TRUE(hsm_trigger_event(&home.lighting_hsm, LIGHT_EVT_TURN_ON));
    ASSERT_EQ(home.lighting_hsm.state, LIGHT_DAY_MODE); // Morning = day mode

    // 3. User leaves for work
    home.sensors.user_present = false;
    ASSERT_TRUE(hsm_trigger_event(&home.main_hsm, MAIN_EVT_USER_LEAVES));
    ASSERT_EQ(home.main_hsm.state, MAIN_AWAY);

    // Verify away mode coordination
    ASSERT_EQ(home.security_hsm.state, SEC_ARMED_AWAY);
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_AUTO); // Should stay in auto for efficiency
    ASSERT_EQ(home.lighting_hsm.state, LIGHT_OFF);   // Lights off to save energy

    // 4. Evening - simulate time change
    home.sensors.time_of_day = 22; // 10 PM

    // 5. Motion detected while away (potential intrusion)
    home.sensors.motion_detected = true;
    ASSERT_TRUE(hsm_trigger_event(&home.security_hsm, SEC_EVT_INTRUSION));
    ASSERT_EQ(home.security_hsm.state, SEC_ALARM);
    ASSERT_TRUE(home.sensors.alarm_triggered);

    // 6. User returns and disarms
    home.sensors.user_present = true;
    ASSERT_TRUE(hsm_trigger_event(&home.security_hsm, SEC_EVT_DISARM));
    ASSERT_EQ(home.security_hsm.state, SEC_DISARMED);

    ASSERT_TRUE(hsm_trigger_event(&home.main_hsm, MAIN_EVT_USER_RETURNS));
    ASSERT_EQ(home.main_hsm.state, MAIN_NORMAL);

    // 7. Evening lighting
    ASSERT_TRUE(hsm_trigger_event(&home.lighting_hsm, LIGHT_EVT_TURN_ON));
    ASSERT_EQ(home.lighting_hsm.state, LIGHT_NIGHT_MODE); // Evening = night mode
    ASSERT_EQ(home.sensors.light_level, 20);

    // 8. Test periodic updates (main controller coordinating subsystems)
    int initial_count = home.transitions_count;
    ASSERT_TRUE(hsm_trigger_event(&home.main_hsm, MAIN_EVT_PERIODIC_UPDATE));
    ASSERT_GT(home.transitions_count, initial_count); // Should have incremented during coordination

    // Verify system is in expected final state
    ASSERT_EQ(home.main_hsm.state, MAIN_NORMAL);
    ASSERT_EQ(home.security_hsm.state, SEC_DISARMED);
    ASSERT_EQ(home.climate_hsm.state, CLIMATE_AUTO);
    ASSERT_EQ(home.lighting_hsm.state, LIGHT_NIGHT_MODE);
    ASSERT_TRUE(home.system_active);
}

//==============================================================================
// Test suite - tests will be run by test_hsm.c main()
//==============================================================================
