#include "debug.h"
#include "print.h"

#include "cyclone.h"

#include "ch.h"
#include "chtrace.h"
#include "chvt.h"
#include "hal.h"
#include "hal_pal.h"

// #define TEST_AUDIO_PIN_OUTPUT

#ifdef OLED_DRIVER_ENABLE
#    include "oled_driver.h"
#endif  // OLED_DRIVER_ENABLE

#ifdef RGB_MATRIX_ENABLE
led_config_t g_led_config = {{// Key Matrix to LED Index
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0},
                              {14, 12, 10, 8, 6, 4, 2, 0}},
                             {// LED Index to Physical Position
                              {224, 64},
                              {208, 64},
                              {192, 64},
                              {176, 64},
                              {160, 64},
                              {144, 64},
                              {128, 64},
                              {112, 64},
                              {96, 64},
                              {80, 64},
                              {64, 64},
                              {48, 64},
                              {32, 64},
                              {16, 64},
                              {0, 64}},
                             {// LED Index to Flag
                              4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4}};
#endif  // RGB_MATRIX_ENABLE

void matrix_init_kb(void) {

    // Set the level shifter GPIO directions and initial states, needs to be before shift registers get initialised
    setPinOutput(PIN_EN_5V_IO_INV);
    setPinOutput(PIN_EN_5V_IO);

    // Enable the 3V3 -> 5V transceivers
    writePinLow(PIN_EN_5V_IO_INV);
    writePinHigh(PIN_EN_5V_IO);

    // Run the rest of the init routines
    matrix_init_user();
}

void keyboard_post_init_kb(void) {
#ifdef EEPROM_ENABLE
    if (!eeconfig_is_enabled()) {
        eeconfig_init();
    }
#endif  // EEPROM_ENABLE

    debug_enable   = false;
    debug_matrix   = false;
    debug_keyboard = false;
    debug_mouse    = false;

#ifdef TEST_AUDIO_PIN_OUTPUT
    setPinOutput(A4);
    setPinOutput(A5);
    for (uint32_t i = 0; i < 2000; ++i) {
        writePinHigh(A4);
        writePinLow(A5);
        wait_ms(1);
        writePinLow(A4);
        writePinHigh(A5);
        wait_ms(1);
    }
#endif  // TEST_AUDIO_PIN_OUTPUT
}

#ifdef OLED_DRIVER_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    // We're vertical...
    return OLED_ROTATION_90;
}

void oled_task_user(void) {
    // Host Keyboard LED Status
    uint8_t led_usb_state = host_keyboard_leds();
    oled_write_ln_P(led_usb_state & (1 << USB_LED_NUM_LOCK) ? PSTR("NUM  ") : PSTR("     "), false);
    oled_advance_page(true);
    oled_write_ln_P(led_usb_state & (1 << USB_LED_CAPS_LOCK) ? PSTR("CAP  ") : PSTR("     "), false);
    oled_advance_page(true);
    oled_write_ln_P(led_usb_state & (1 << USB_LED_SCROLL_LOCK) ? PSTR("SCR  ") : PSTR("     "), false);
}
#endif  // OLED_DRIVER_ENABLE

void chibi_system_halt_hook(const char* reason) {
    // re-route to QMK toolbox...
    uprintf("system halting: %s\n", reason);
}

void chibi_system_trace_hook(void* tep) {
    // re-route to QMK toolbox...
    uprintf("trace\n");
}

void chibi_debug_check_hook(const char* func, const char* condition, int value) {
    // re-route to QMK toolbox...
    uprintf("%s debug check failure: (%s) == %s\n", func, condition, value ? "true" : "false");
    // ...and hard-loop fail
    while (1) {
    }
}

void chibi_debug_assert_hook(const char* func, const char* condition, int value, const char* reason) {
    // re-route to QMK toolbox...
    uprintf("%s debug assert (%s) failure: (%s) == %s\n", func, reason, condition, value ? "true" : "false");
    // ...and hard-loop fail
    while (1) {
    }
}
