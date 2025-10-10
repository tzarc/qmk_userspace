// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

/*
    STM32F411 BlackPill USB HID Keyboard
    Main Application
*/

#include "ch.h"
#include "hal.h"
#include "usbcfg.h"
#include <string.h>

/* User button pin (PA0 on BlackPill - KEY button) */
#define BUTTON_GPIO GPIOA
#define BUTTON_PIN 0

/* NKRO keyboard report buffer */
static volatile nkro_report_t kbd_report       = {0};
static volatile systime_t     last_button_time = 0;

/* Binary semaphore to signal report ready for transmission */
static binary_semaphore_t report_ready_sem;

#define DEBOUNCE_TIME_MS 50

/*
 * EXTI callback for button press/release
 * Updates keyboard report and triggers USB remote wakeup if suspended
 */
static void button_exti_cb(void *arg) {
    (void)arg;

    systime_t now = chVTGetSystemTimeX();

    /* Debounce check */
    if (chVTTimeElapsedSinceX(last_button_time) < TIME_MS2I(DEBOUNCE_TIME_MS)) {
        return;
    }

    last_button_time = now;

    /* Read current button state (active low) */
    bool button_pressed = (palReadPad(BUTTON_GPIO, BUTTON_PIN) == PAL_LOW);

    /* Update keyboard report using NKRO bitfield */
    if (button_pressed) {
        /* Button pressed - set 'a' (HID keycode 0x04) in NKRO bitfield */
        NKRO_SET_KEY((nkro_report_t *)&kbd_report, 0x04);
    } else {
        /* Button released - clear 'a' key from NKRO bitfield */
        NKRO_CLEAR_KEY((nkro_report_t *)&kbd_report, 0x04);
    }

    /* Update boot protocol keys for BIOS compatibility
     * Boot protocol hosts ignore the descriptor and read bytes 0-7 directly.
     * NKRO hosts now correctly ignore boot_keys (no usages assigned in descriptor).
     * This maintains compatibility with both BIOS and modern OS. */
    nkro_update_boot_keys((nkro_report_t *)&kbd_report);

    /* Signal semaphore to wake transmission thread */
    chSysLockFromISR();
    chBSemSignalI(&report_ready_sem);
    chSysUnlockFromISR();

    /* If USB is suspended, trigger remote wakeup */
    if (usb_suspended) {
        usb_remote_wakeup();
    }
}

/*
 * USB HID keyboard transmission thread
 * Fully interrupt-driven - waits on semaphore signaled by button interrupt
 */
static THD_WORKING_AREA(waThread1, 256);
static THD_FUNCTION(Thread1, arg) {
    (void)arg;
    chRegSetThreadName("usb_tx");

    /* Wait for USB to be ready */
    chThdSleepMilliseconds(3000);

    while (true) {
        /* Wait for semaphore signal from button interrupt */
        chBSemWait(&report_ready_sem);

        /* Only transmit if not suspended */
        if (!usb_suspended) {
            /* Copy report to local buffer for transmission */
            nkro_report_t report;
            chSysLock();
            report = kbd_report;
            chSysUnlock();

            /* Transmit the NKRO report (21 bytes) */
            usbTransmit(&USBD1, 1, (uint8_t *)&report, NKRO_REPORT_SIZE);
        }
    }
}

/*
 * Application entry point
 */
int main(void) {
    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();

    /*
     * Explicitly clear the keyboard report to ensure no garbage data
     */
    memset((void *)&kbd_report, 0, sizeof(kbd_report));

    /*
     * Initialize binary semaphore (taken state - thread will wait)
     */
    chBSemObjectInit(&report_ready_sem, TRUE);

    /*
     * Initializes the user button pin with interrupt on both edges
     * This allows detection of both press and release events
     */
    palSetLineCallback(PAL_LINE(BUTTON_GPIO, BUTTON_PIN), button_exti_cb, NULL);
    palEnableLineEvent(PAL_LINE(BUTTON_GPIO, BUTTON_PIN), PAL_EVENT_MODE_BOTH_EDGES);

    /*
     * Initialize USB
     */
    usb_start();

    /*
     * Creates the USB transmission thread
     */
    chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

    /*
     * Normal main() thread activity, in this demo it does nothing
     */
    while (true) {
        chThdSleepMilliseconds(500);
    }
}
