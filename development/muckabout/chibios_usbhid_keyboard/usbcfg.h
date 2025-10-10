// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

/*
    STM32F411 BlackPill USB HID Keyboard
    USB Configuration Header
*/

#ifndef USBCFG_H
#define USBCFG_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

extern const USBConfig usbcfg;
extern USBDriver       USBD1;

/* USB suspend/resume state */
extern volatile bool usb_suspended;

/* NKRO HID Report Structure (21 bytes total)
 *
 * Compatible with boot protocol (bytes 0-7) and NKRO bitfield (bytes 8-20).
 * Boot protocol hosts read bytes 0-7 as standard boot keyboard format.
 * NKRO-aware hosts use the bitfield in bytes 8-20.
 */
typedef struct {
    uint8_t modifiers;     // Byte 0: Modifier keys bitfield
    uint8_t reserved;      // Byte 1: Reserved (constant)
    uint8_t boot_keys[6];  // Bytes 2-7: Boot protocol keycodes (declared as padding)
    uint8_t nkro_bits[13]; // Bytes 8-20: NKRO bitfield (104 keys, 0x00-0x67)
} __attribute__((packed)) nkro_report_t;

#define NKRO_REPORT_SIZE sizeof(nkro_report_t) // 21 bytes

/* Helper macros for NKRO bitfield manipulation */
#define NKRO_SET_KEY(report, keycode)                                     \
    do {                                                                  \
        if ((keycode) <= 0x67) {                                          \
            (report)->nkro_bits[(keycode) / 8] |= (1 << ((keycode) % 8)); \
        }                                                                 \
    } while (0)

#define NKRO_CLEAR_KEY(report, keycode)                                    \
    do {                                                                   \
        if ((keycode) <= 0x67) {                                           \
            (report)->nkro_bits[(keycode) / 8] &= ~(1 << ((keycode) % 8)); \
        }                                                                  \
    } while (0)

#define NKRO_IS_KEY_SET(report, keycode) (((keycode) <= 0x67) && ((report)->nkro_bits[(keycode) / 8] & (1 << ((keycode) % 8))))

/* Helper function to populate boot protocol keys from NKRO bitfield
 * This maintains boot protocol compatibility by copying up to 6 pressed keys
 * into the boot_keys array. Boot protocol hosts will read these.
 */
static inline void nkro_update_boot_keys(nkro_report_t *report) {
    uint8_t boot_idx = 0;
    memset(report->boot_keys, 0, sizeof(report->boot_keys));

    for (uint8_t keycode = 0; keycode <= 0x67 && boot_idx < 6; keycode++) {
        if (NKRO_IS_KEY_SET(report, keycode)) {
            report->boot_keys[boot_idx++] = keycode;
        }
    }
}

/* USB HID Keyboard specific functions */
void usb_start(void);
void usb_remote_wakeup(void);

#endif /* USBCFG_H */
