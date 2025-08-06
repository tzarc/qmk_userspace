// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "hsm.h"

typedef struct vehicle_data_t {
    int         speed;
    bool        brake_pressed;
    bool        accelerator_pressed;
    hsm_event_t pending_event;
    int         time_in_state;
} vehicle_data_t;

void drive_entry(void *state_data);
void neutral_entry(void *state_data);
void reverse_entry(void *state_data);

void drive_action(void *state_data);
void neutral_action(void *state_data);
void reverse_action(void *state_data);

void drive_exit(void *state_data);
void neutral_exit(void *state_data);
void reverse_exit(void *state_data);

bool can_shift_to_neutral(void *state_data);
bool can_shift_to_reverse(void *state_data);

void        simulate_user_input(vehicle_data_t *vehicle);
const char *state_name(hsm_state_t state);
const char *event_name(hsm_event_t event);

enum { DRIVE, NEUTRAL, REVERSE };
enum { EVT_SHIFT_UP, EVT_SHIFT_DOWN, EVT_ACCELERATE, EVT_BRAKE, EVT_COAST };

int main(int argc, char *const argv[]) {
    srand(time(NULL));

    const hsm_entry_t state_machine[] = {
        [DRIVE]   = {drive_entry, drive_action, drive_exit},
        [NEUTRAL] = {neutral_entry, neutral_action, neutral_exit},
        [REVERSE] = {reverse_entry, reverse_action, reverse_exit},
    };

    const hsm_transition_t transitions[] = {
        // From NEUTRAL
        {NEUTRAL, EVT_SHIFT_UP, DRIVE, NULL},
        {NEUTRAL, EVT_SHIFT_DOWN, REVERSE, can_shift_to_reverse},

        // From DRIVE
        {DRIVE, EVT_SHIFT_DOWN, NEUTRAL, can_shift_to_neutral},

        // From REVERSE
        {REVERSE, EVT_SHIFT_UP, NEUTRAL, can_shift_to_neutral},
    };

    vehicle_data_t vehicle = {
        .speed               = 0,
        .brake_pressed       = false,
        .accelerator_pressed = false,
        .pending_event       = EVT_COAST,
        .time_in_state       = 0,
    };

    hsm_t hsm;
    if (!HSM_INIT(&hsm, state_machine, transitions, NEUTRAL, &vehicle)) {
        printf("ERROR: Failed to initialize HSM!\n");
        return 1;
    }

    printf("=== Vehicle State Machine Demo ===\n");
    printf("Initial state: %s\n\n", state_name(hsm.state));

    int              iterations     = 0;
    static const int max_iterations = 30;

    while (iterations < max_iterations) {
        simulate_user_input(&vehicle);
        vehicle.time_in_state++;

        printf("Iteration %d: State=%s, Speed=%d, Brake=%s, Accel=%s\n", iterations + 1, state_name(hsm.state), vehicle.speed, vehicle.brake_pressed ? "ON" : "OFF", vehicle.accelerator_pressed ? "ON" : "OFF");

        // Trigger the pending event from user input simulation
        if (vehicle.pending_event == EVT_SHIFT_UP) {
            printf("  -> Triggering SHIFT_UP event\n");
        } else if (vehicle.pending_event == EVT_SHIFT_DOWN) {
            printf("  -> Triggering SHIFT_DOWN event\n");
        }

        if (!hsm_trigger_event(&hsm, vehicle.pending_event)) {
            printf("ERROR: Event triggering failed!\n");
            break;
        }

        iterations++;
        printf("\n");
    }

    printf("Final state: %s, Final speed: %d\n", state_name(hsm.state), vehicle.speed);
    return 0;
}

const char *state_name(hsm_state_t state) {
    switch (state) {
        case DRIVE:
            return "DRIVE";
        case NEUTRAL:
            return "NEUTRAL";
        case REVERSE:
            return "REVERSE";
        default:
            return "UNKNOWN";
    }
}

const char *event_name(hsm_event_t event) {
    switch (event) {
        case EVT_SHIFT_UP:
            return "SHIFT_UP";
        case EVT_SHIFT_DOWN:
            return "SHIFT_DOWN";
        case EVT_ACCELERATE:
            return "ACCELERATE";
        case EVT_BRAKE:
            return "BRAKE";
        case EVT_COAST:
            return "COAST";
        default:
            return "UNKNOWN";
    }
}

void simulate_user_input(vehicle_data_t *vehicle) {
    // Simulate random user inputs
    vehicle->brake_pressed       = (rand() % 4) == 0; // 25% chance
    vehicle->accelerator_pressed = (rand() % 3) == 0; // 33% chance

    // Don't press brake and accelerator at same time
    if (vehicle->brake_pressed) {
        vehicle->accelerator_pressed = false;
    }

    // Determine event based on inputs
    vehicle->pending_event = EVT_COAST; // Default event
    if (vehicle->brake_pressed) {
        vehicle->pending_event = EVT_BRAKE;
    } else if (vehicle->accelerator_pressed) {
        vehicle->pending_event = EVT_ACCELERATE;
    }

    // Randomly trigger shift events (overrides other events)
    if ((rand() % 10) == 0) { // 10% chance
        if ((rand() % 2) == 0) {
            vehicle->pending_event = EVT_SHIFT_UP;
        } else {
            vehicle->pending_event = EVT_SHIFT_DOWN;
        }
    }
}

void drive_entry(void *state_data) {
    vehicle_data_t *vehicle = (vehicle_data_t *)state_data;
    printf("  -> Entering DRIVE state\n");
    vehicle->time_in_state = 0;
}

void neutral_entry(void *state_data) {
    vehicle_data_t *vehicle = (vehicle_data_t *)state_data;
    printf("  -> Entering NEUTRAL state\n");
    vehicle->time_in_state = 0;
}

void reverse_entry(void *state_data) {
    vehicle_data_t *vehicle = (vehicle_data_t *)state_data;
    printf("  -> Entering REVERSE state\n");
    vehicle->time_in_state = 0;
}

void drive_exit(void *state_data) {
    vehicle_data_t *vehicle = (vehicle_data_t *)state_data;
    printf("  <- Exiting DRIVE state (spent %d iterations, final speed: %d)\n", vehicle->time_in_state, vehicle->speed);
}

void neutral_exit(void *state_data) {
    vehicle_data_t *vehicle = (vehicle_data_t *)state_data;
    printf("  <- Exiting NEUTRAL state (spent %d iterations)\n", vehicle->time_in_state);
}

void reverse_exit(void *state_data) {
    vehicle_data_t *vehicle = (vehicle_data_t *)state_data;
    printf("  <- Exiting REVERSE state (spent %d iterations, final speed: %d)\n", vehicle->time_in_state, vehicle->speed);
}

void drive_action(void *state_data) {
    vehicle_data_t *vehicle = (vehicle_data_t *)state_data;

    // Update speed based on inputs
    if (vehicle->accelerator_pressed && !vehicle->brake_pressed) {
        vehicle->speed += 5;
        if (vehicle->speed > 60) vehicle->speed = 60; // Max speed
    } else if (vehicle->brake_pressed) {
        vehicle->speed -= 10;
        if (vehicle->speed < 0) vehicle->speed = 0;
    } else {
        // Coasting - gradual slowdown
        if (vehicle->speed > 0) {
            vehicle->speed -= 2;
            if (vehicle->speed < 0) vehicle->speed = 0;
        }
    }
}

void neutral_action(void *state_data) {
    vehicle_data_t *vehicle = (vehicle_data_t *)state_data;

    // In neutral, speed gradually decreases due to friction
    if (vehicle->speed > 0) {
        vehicle->speed -= 3;
        if (vehicle->speed < 0) vehicle->speed = 0;
    }
}

void reverse_action(void *state_data) {
    vehicle_data_t *vehicle = (vehicle_data_t *)state_data;

    // Update speed based on inputs (negative speed for reverse)
    if (vehicle->accelerator_pressed && !vehicle->brake_pressed) {
        vehicle->speed -= 3;                            // Going backwards
        if (vehicle->speed < -20) vehicle->speed = -20; // Max reverse speed
    } else if (vehicle->brake_pressed) {
        if (vehicle->speed < 0) {
            vehicle->speed += 8; // Braking while reversing
            if (vehicle->speed > 0) vehicle->speed = 0;
        }
    } else {
        // Coasting - gradual slowdown
        if (vehicle->speed < 0) {
            vehicle->speed += 2;
            if (vehicle->speed > 0) vehicle->speed = 0;
        }
    }
}

bool can_shift_to_neutral(void *state_data) {
    vehicle_data_t *vehicle = (vehicle_data_t *)state_data;
    return vehicle->speed < 5 && vehicle->speed >= 0; // Can shift to neutral at low forward speeds
}

bool can_shift_to_reverse(void *state_data) {
    vehicle_data_t *vehicle = (vehicle_data_t *)state_data;
    return vehicle->speed == 0; // Can only shift to reverse when stopped
}
