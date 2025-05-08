// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "analog.h"

#include "layouts/ortho_4x4/tzarc/keymap.c"

bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TZ_CUST1: {
            break;
        }
    }
    return true;
}

void housekeeping_task_keymap(void) {
    static uint32_t last_read    = 0;
    static uint8_t  current_slot = 0;
    if (timer_elapsed32(last_read) < 10) {
        return;
    }
    last_read = timer_read32();

    static int16_t voltage_readings[100] = {0};
    static int16_t current_readings[100] = {0};
    static int32_t voltage_sum           = 0;
    static int32_t current_sum           = 0;

    int16_t voltage_mv = analogReadPin(ADC_VOLTAGE_PIN);
    int16_t current_ma = analogReadPin(ADC_CURRENT_PIN);

    // Remove the last contribution in this slot
    voltage_sum -= voltage_readings[current_slot];
    current_sum -= current_readings[current_slot];

    // Store the new readings
    voltage_readings[current_slot] = voltage_mv;
    current_readings[current_slot] = current_ma;

    // Add the new readings to the sum
    voltage_sum += voltage_mv;
    current_sum += current_ma;

    // Increment the read index
    current_slot = (current_slot + 1) % 100;

    if (current_slot == 99) {
        int16_t voltage_avg = voltage_sum / 100;
        int16_t current_avg = current_sum / 100;
        voltage_mv          = (2 * (3300 * voltage_avg)) / ADC_SATURATION;
        current_ma          = (3300 * current_avg) / ADC_SATURATION / 10; // current is wrongly at a 10x multiplier in Kobold R1, so divide by 10
        dprintf("Voltage: %dmV -- Current: %dmA\n", (int)voltage_mv, (int)current_ma);
    }
}
