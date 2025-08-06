// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "hsm.h"

typedef struct vehicle_data_t {
    int  speed;
    bool brake_pressed;
    bool accelerator_pressed;
    bool shift_up_requested;
    bool shift_down_requested;
    int  time_in_state;
} vehicle_data_t;

hsm_state_t drive(void *state_data);
hsm_state_t neutral(void *state_data);
hsm_state_t reverse(void *state_data);

void drive_entry(void *state_data);
void neutral_entry(void *state_data);
void reverse_entry(void *state_data);

void drive_exit(void *state_data);
void neutral_exit(void *state_data);
void reverse_exit(void *state_data);

void        simulate_user_input(vehicle_data_t *vehicle);
const char *state_name(hsm_state_t state);

enum { DRIVE, NEUTRAL, REVERSE };

int main(int argc, char *const argv[]) {
    srand(time(NULL));

    const hsm_entry_t state_machine[] = {
        [DRIVE]   = {drive_entry, drive, drive_exit},
        [NEUTRAL] = {neutral_entry, neutral, neutral_exit},
        [REVERSE] = {reverse_entry, reverse, reverse_exit},
    };

    vehicle_data_t vehicle = {.speed = 0, .brake_pressed = false, .accelerator_pressed = false, .shift_up_requested = false, .shift_down_requested = false, .time_in_state = 0};

    hsm_t hsm;
    if (!HSM_INIT(&hsm, state_machine, NEUTRAL, &vehicle)) {
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

        printf("Iteration %d: State=%s, Speed=%d, Brake=%s, Accel=%s, ShiftUp=%s, ShiftDown=%s\n", iterations + 1, state_name(hsm.state), vehicle.speed, vehicle.brake_pressed ? "ON" : "OFF", vehicle.accelerator_pressed ? "ON" : "OFF", vehicle.shift_up_requested ? "YES" : "NO", vehicle.shift_down_requested ? "YES" : "NO");

        if (!hsm_execute(&hsm)) {
            printf("ERROR: HSM execution failed!\n");
            break;
        }
        iterations++;

        // Reset shift requests after processing
        vehicle.shift_up_requested   = false;
        vehicle.shift_down_requested = false;

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

void simulate_user_input(vehicle_data_t *vehicle) {
    // Simulate random user inputs
    vehicle->brake_pressed        = (rand() % 4) == 0; // 25% chance
    vehicle->accelerator_pressed  = (rand() % 3) == 0; // 33% chance
    vehicle->shift_up_requested   = (rand() % 8) == 0; // 12.5% chance
    vehicle->shift_down_requested = (rand() % 8) == 0; // 12.5% chance

    // Don't press brake and accelerator at same time
    if (vehicle->brake_pressed) {
        vehicle->accelerator_pressed = false;
    }

    // Don't request both shifts at same time
    if (vehicle->shift_up_requested) {
        vehicle->shift_down_requested = false;
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

hsm_state_t drive(void *state_data) {
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

    // State transitions
    if (vehicle->shift_down_requested && vehicle->speed < 5) {
        return NEUTRAL;
    }

    // Stay in drive
    return DRIVE;
}

hsm_state_t neutral(void *state_data) {
    vehicle_data_t *vehicle = (vehicle_data_t *)state_data;

    // In neutral, speed gradually decreases due to friction
    if (vehicle->speed > 0) {
        vehicle->speed -= 3;
        if (vehicle->speed < 0) vehicle->speed = 0;
    }

    // State transitions
    if (vehicle->shift_up_requested) {
        return DRIVE;
    } else if (vehicle->shift_down_requested && vehicle->speed == 0) {
        return REVERSE;
    }

    // Stay in neutral
    return NEUTRAL;
}

hsm_state_t reverse(void *state_data) {
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

    // State transitions
    if (vehicle->shift_up_requested && vehicle->speed == 0) {
        return NEUTRAL;
    }

    // Stay in reverse
    return REVERSE;
}
