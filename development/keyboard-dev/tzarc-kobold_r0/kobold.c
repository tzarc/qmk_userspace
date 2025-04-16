// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include <qp.h>

#ifdef QUANTUM_PAINTER_ENABLE
painter_device_t oled;

__attribute__((weak)) bool display_init_user(void) {
    return false;
}

__attribute__((weak)) bool display_task_user(void) {
    return false;
}

bool display_init_kb(void) {
    // Initialize OLED display
    oled = qp_sh1106_make_i2c_device(128, 32, 0x3C);
    qp_init(oled, QP_ROTATION_0);
    qp_power(oled, true);

    // Offload to user display
    if (!display_init_user()) {
        return true;
    }

    // Keyboard-level display defaults
    qp_circle(oled, 8, 8, 8, 0, 0, 255, true);

    return true;
}

bool display_task_kb(void) {
    // Offload to user display
    if (!display_task_user()) {
        return true;
    }

    // Keyboard-level display defaults
    // - No-op.
    return true;
}
#endif // QUANTUM_PAINTER_ENABLE

void configure_ina219(void);

void board_init(void) {
    // Disable RGB LEDs
    gpio_set_pin_output(RGB_ENABLE_PIN);
    gpio_write_pin(RGB_ENABLE_PIN, 1);
}

void keyboard_post_init_kb() {
#ifdef QUANTUM_PAINTER_ENABLE
    display_init_kb();
#endif // QUANTUM_PAINTER_ENABLE

    keyboard_post_init_user();
}

void housekeeping_task_kb() {
    // Enable RGB LEDs after 200 milliseconds
    static bool rgb_enabled = false;
    if (!rgb_enabled && timer_read32() > 200) {
        gpio_write_pin(RGB_ENABLE_PIN, 0);
        rgb_enabled = true;
    }

#ifdef QUANTUM_PAINTER_ENABLE
    static uint32_t last_display_task = 0;
    if (timer_elapsed32(last_display_task) > 1000) {
        last_display_task = timer_read32();
        display_task_kb();
        qp_flush(oled);
    }
#endif // QUANTUM_PAINTER_ENABLE
}

void configure_ina219(void) {
    /*
    VBUS_MAX = 16V
    VSHUNT_MAX = 0.04V
    RSHUNT = 0.002 ohms

    Max possible current:
    MaxPossible_I = VSHUNT_MAX / RSHUNT = 20A

    Expected max current:
    MaxExpected_I = 3A

    Calculate range of LSBs
    MinimumLSB = MaxExpected_I / 32767 = 0.0000914 (91.55uA per bit)
    MaximumLSB = MaxExpected_I / 4096 = 0.000732 (732uA per bit)

    Choose LSB between min and max, choose roundish number close to MinimumLSB
    CurrentLSB = 0.001 (1mA per bit)

    Compute calibration register
    Cal = trunc(0.04096 / (CurrentLSB * RSHUNT))
    Cal = 20480

    */
}
