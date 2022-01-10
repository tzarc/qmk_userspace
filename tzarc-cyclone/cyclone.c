// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "debug.h"
#include "print.h"

#include "cyclone.h"

#include "ch.h"
#include "chtrace.h"
#include "chvt.h"
#include "hal.h"
#include "hal_pal.h"

// #define TEST_AUDIO_PIN_OUTPUT
// #define TEST_SPI_OUTPUT

#ifdef OLED_DRIVER_ENABLE
#    include "oled_driver.h"
#endif  // OLED_DRIVER_ENABLE

#ifdef QWIIC_MICRO_OLED_ENABLE
#    include "micro_oled.h"
static void draw_ui(void);
#endif  // QWIIC_MICRO_OLED_ENABLE

#ifdef QUANTUM_PAINTER_ENABLE
static void draw_ui(void);
#endif  // QUANTUM_PAINTER_ENABLE

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

void matrix_io_delay(void) { __asm__ volatile("nop\nnop\nnop\n"); }

void housekeeping_task_kb(void) {
#ifdef TEST_SPI_OUTPUT
    static const SPIConfig hs_spicfg = {0, NULL, GPIOA, 1, SPI_CR1_BR_0, 0};

    static uint32_t last = 0;
    uint32_t        now  = timer_read32();
    if (now - last > 20) {
        const char txbuf[4] = {0x12, 0x34, 0x56, 0x78};
        char       rxbuf[4] = {0};
        last                = now;

        spiStart(&SPID1, &hs_spicfg);         /* Setup transfer parameters.       */
        spiSelect(&SPID1);                    /* Slave Select assertion.          */
        spiExchange(&SPID1, 4, txbuf, rxbuf); /* Atomic transfer operations.      */
        spiUnselect(&SPID1);                  /* Slave Select de-assertion.       */
        spiStop(&SPID1);
    }
#endif

#if defined(QWIIC_MICRO_OLED_ENABLE) || defined(QUANTUM_PAINTER_ENABLE)
    (void)draw_ui;
    draw_ui();
#endif  // defined(QWIIC_MICRO_OLED_ENABLE) || defined(QUANTUM_PAINTER_ENABLE)

    /*
    static uint32_t        last_oled_timer_print = 0;
    extern uint32_t oled_timeout;
    uint32_t now = timer_read32();
    if (now - last_oled_timer_print > 1000) {
        last_oled_timer_print = now;
        uprintf("%6u/%6u/%6u\n", now, oled_timeout, oled_timeout - now);
    }
    */
}

void matrix_init_kb(void) {
#ifdef TEST_SPI_OUTPUT
    palSetPadMode(GPIOB, 3, PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST); /* New SCK.     */
    palSetPadMode(GPIOB, 4, PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST); /* New MISO.    */
    palSetPadMode(GPIOB, 5, PAL_MODE_ALTERNATE(0) | PAL_STM32_OSPEED_HIGHEST); /* New MOSI.    */
    palSetPad(GPIOA, 1);
    palSetPadMode(GPIOA, 1, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST); /* New CS.      */
#endif

    palSetPadMode(GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOB, 0);

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

    debug_enable   = true;
    debug_matrix   = true;
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

#if defined(QUANTUM_PAINTER_ENABLE)
#    include <qp.h>
#    include <qp_qmk_oled_wrapper.h>
#    include "graphics/lock-caps.c"
#    include "graphics/lock-num.c"
#    include "graphics/lock-scrl.c"
#    include "graphics/lock-caps-OFF.c"
#    include "graphics/lock-num-OFF.c"
#    include "graphics/lock-scrl-OFF.c"
#    include "graphics/noto.c"
void draw_ui(void) {
    static painter_device_t oled = NULL;
    if (!oled) {
        oled = qp_qmk_oled_wrapper_make_device();
        qp_init(oled, QP_ROTATION_0);
        qp_power(oled, true);
    }

    static uint32_t last_draw = 0;
    if (timer_elapsed32(last_draw) > 2500) {
        last_draw = timer_read32();
        qp_line(oled, 0, 0, 127, 31, HSV_WHITE);
        qp_circle(oled, 10, 22, 8, HSV_WHITE, true);
        qp_ellipse(oled, 107, 10, 18, 8, HSV_WHITE, false);
        qp_drawtext(oled, 0, 0, font_noto, "Hello there");
        qp_drawtext(oled, 0, font_noto->glyph_height, font_noto, "General Kenobi");
    }

    static led_t last_led_state = {0};
    led_t        led_state      = host_keyboard_led_state();
    if (last_led_state.raw != led_state.raw) {
        last_led_state.raw = led_state.raw;
        qp_drawimage(oled, 0, 32, last_led_state.caps_lock ? gfx_lock_caps : gfx_lock_caps_OFF);
        qp_drawimage(oled, 32, 32, last_led_state.num_lock ? gfx_lock_num : gfx_lock_num_OFF);
        qp_drawimage(oled, 64, 32, last_led_state.scroll_lock ? gfx_lock_scrl : gfx_lock_scrl_OFF);
    }
}
#endif  // defined(QUANTUM_PAINTER_ENABLE)

#if defined(OLED_DRIVER_ENABLE) && !defined(QUANTUM_PAINTER_ENABLE)
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    // We're vertical...
    return OLED_ROTATION_270;
}

uint8_t last_led_usb_state = 0xFF;
void    oled_task_user(void) {
    // Host Keyboard LED Status
    uint8_t led_usb_state = host_keyboard_leds();
    if (last_led_usb_state != led_usb_state) {
        last_led_usb_state = led_usb_state;
        oled_write_ln_P(led_usb_state & (1 << USB_LED_NUM_LOCK) ? PSTR("NUM  ") : PSTR("     "), false);
        oled_advance_page(true);
        oled_write_ln_P(led_usb_state & (1 << USB_LED_CAPS_LOCK) ? PSTR("CAP  ") : PSTR("     "), false);
        oled_advance_page(true);
        oled_write_ln_P(led_usb_state & (1 << USB_LED_SCROLL_LOCK) ? PSTR("SCR  ") : PSTR("     "), false);
    }
}
#endif  // defined(OLED_DRIVER_ENABLE) && !defined(QUANTUM_PAINTER_ENABLE)

#ifdef QWIIC_MICRO_OLED_ENABLE
uint8_t last_led_usb_state = 0xFF;
void    draw_ui(void) {
    uint8_t led_usb_state = host_keyboard_leds();
    if (last_led_usb_state != led_usb_state) {
        last_led_usb_state = led_usb_state;
        draw_string(2, 2, led_usb_state & (1 << USB_LED_NUM_LOCK) ? PSTR("NUM  ") : PSTR("     "), PIXEL_ON, NORM, 0);
        draw_string(2, 12, led_usb_state & (1 << USB_LED_CAPS_LOCK) ? PSTR("CAPS") : PSTR("     "), PIXEL_ON, NORM, 0);
        draw_string(2, 22, led_usb_state & (1 << USB_LED_SCROLL_LOCK) ? PSTR("SCRL") : PSTR("     "), PIXEL_ON, NORM, 0);
    }
    send_buffer();
}
#endif  // QWIIC_MICRO_OLED_ENABLE

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
