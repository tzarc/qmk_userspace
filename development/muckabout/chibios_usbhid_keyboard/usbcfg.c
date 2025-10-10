// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

/*
    STM32F411 BlackPill USB HID Keyboard
    USB Configuration Implementation
*/

#include "hal.h"
#include "usbcfg.h"

/* USB Driver (defined in hal_usb_lld.c) */
extern USBDriver USBD1;

/* USB suspend state flag */
volatile bool usb_suspended = false;

/*
 * USB Device Descriptor
 */
static const uint8_t vcom_device_descriptor_data[18] = {
    USB_DESC_DEVICE(0x0200, /* bcdUSB (2.0).                    */
                    0x00,   /* bDeviceClass (defined in interface). */
                    0x00,   /* bDeviceSubClass.                */
                    0x00,   /* bDeviceProtocol.                */
                    0x40,   /* bMaxPacketSize.                 */
                    0x1209, /* idVendor (pid.codes).           */
                    0x0001, /* idProduct.                      */
                    0x0200, /* bcdDevice.                      */
                    1,      /* iManufacturer.                  */
                    2,      /* iProduct.                       */
                    3,      /* iSerialNumber.                  */
                    1)      /* bNumConfigurations.             */
};

/*
 * Device Descriptor wrapper
 */
static const USBDescriptor vcom_device_descriptor = {sizeof vcom_device_descriptor_data, vcom_device_descriptor_data};

/* HID Report Descriptor - NKRO Keyboard with Boot Protocol Compatibility
 *
 * Report Structure (21 bytes total):
 *   Byte 0:      Modifier keys bitfield (8 bits)
 *   Byte 1:      Reserved (constant)
 *   Bytes 2-7:   "Padding" (appears as 6 keycodes to boot protocol hosts)
 *   Bytes 8-20:  NKRO bitfield (104 bits for keys 0x00-0x67)
 *
 * Boot protocol hosts read bytes 0-7 as standard boot keyboard format.
 * NKRO-aware hosts read the full report and use the bitfield in bytes 8-20.
 */
static const uint8_t hid_report_descriptor_data[] = {
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x06, // USAGE (Keyboard)
    0xa1, 0x01, // COLLECTION (Application)

    // Modifier keys (byte 0)
    0x05, 0x07, //   USAGE_PAGE (Keyboard)
    0x19, 0xe0, //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7, //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00, //   LOGICAL_MINIMUM (0)
    0x25, 0x01, //   LOGICAL_MAXIMUM (1)
    0x75, 0x01, //   REPORT_SIZE (1)
    0x95, 0x08, //   REPORT_COUNT (8)
    0x81, 0x02, //   INPUT (Data,Var,Abs)

    // Reserved byte (byte 1)
    0x95, 0x01, //   REPORT_COUNT (1)
    0x75, 0x08, //   REPORT_SIZE (8)
    0x81, 0x03, //   INPUT (Cnst,Var,Abs) - constant

    // "Padding" bytes 2-7 (appears as 6 keycodes to boot protocol)
    // Declared as constant with NO usages so NKRO hosts completely ignore them
    0x95, 0x06, //   REPORT_COUNT (6)
    0x75, 0x08, //   REPORT_SIZE (8)
    0x81, 0x01, //   INPUT (Cnst,Ary,Abs) - constant array padding

    // NKRO bitfield (bytes 8-20): 104 bits for keys 0x00-0x67
    0x05, 0x07, //   USAGE_PAGE (Keyboard)
    0x19, 0x00, //   USAGE_MINIMUM (0)
    0x29, 0x67, //   USAGE_MAXIMUM (103)
    0x15, 0x00, //   LOGICAL_MINIMUM (0)
    0x25, 0x01, //   LOGICAL_MAXIMUM (1)
    0x75, 0x01, //   REPORT_SIZE (1)
    0x95, 0x68, //   REPORT_COUNT (104)
    0x81, 0x02, //   INPUT (Data,Var,Abs)

    // LED output report (unchanged)
    0x95, 0x05, //   REPORT_COUNT (5)
    0x75, 0x01, //   REPORT_SIZE (1)
    0x05, 0x08, //   USAGE_PAGE (LEDs)
    0x19, 0x01, //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05, //   USAGE_MAXIMUM (Kana)
    0x91, 0x02, //   OUTPUT (Data,Var,Abs)
    0x95, 0x01, //   REPORT_COUNT (1)
    0x75, 0x03, //   REPORT_SIZE (3)
    0x91, 0x03, //   OUTPUT (Cnst,Var,Abs)

    0xc0 // END_COLLECTION
};

static const USBDescriptor hid_report_descriptor = {sizeof hid_report_descriptor_data, hid_report_descriptor_data};

#define HID_DESCRIPTOR_SIZE 9

#define USB_INTERFACE_DESCRIPTOR_SIZE (USB_DESC_INTERFACE_SIZE + HID_DESCRIPTOR_SIZE + USB_DESC_ENDPOINT_SIZE)

/* Configuration Descriptor tree for a HID device */
static const uint8_t hid_configuration_descriptor_data[] = {
    /* Configuration Descriptor */
    USB_DESC_CONFIGURATION(USB_DESC_CONFIGURATION_SIZE + USB_INTERFACE_DESCRIPTOR_SIZE, /* wTotalLength.   */
                           0x01,                                                        /* bNumInterfaces.              */
                           0x01,                                                        /* bConfigurationValue.         */
                           0,                                                           /* iConfiguration.              */
                           0xA0,                                                        /* bmAttributes (bus powered, remote wakeup). */
                           50),                                                         /* bMaxPower (100mA).           */
    /* Interface Descriptor */
    USB_DESC_INTERFACE(0x00, /* bInterfaceNumber.            */
                       0x00, /* bAlternateSetting.           */
                       0x01, /* bNumEndpoints.               */
                       0x03, /* bInterfaceClass (HID).       */
                       0x01, /* bInterfaceSubClass (Boot).   */
                       0x01, /* bInterfaceProtocol (Keyboard). */
                       0),   /* iInterface.                  */
    /* HID Descriptor */
    USB_DESC_BYTE(9),                                  /* bLength.                     */
    USB_DESC_BYTE(0x21),                               /* bDescriptorType (HID).       */
    USB_DESC_BCD(0x0111),                              /* bcdHID.                      */
    USB_DESC_BYTE(0),                                  /* bCountryCode.                */
    USB_DESC_BYTE(1),                                  /* bNumDescriptors.             */
    USB_DESC_BYTE(0x22),                               /* bDescriptorType (Report).    */
    USB_DESC_WORD(sizeof(hid_report_descriptor_data)), /* wDescriptorLength. */
    /* Endpoint 1 Descriptor */
    USB_DESC_ENDPOINT(USB_ENDPOINT_IN(1), /* bEndpointAddress.   */
                      0x03,               /* bmAttributes (Interrupt).    */
                      0x0040,             /* wMaxPacketSize.              */
                      0x0A)               /* bInterval (10ms).            */
};

/*
 * Configuration Descriptor wrapper
 */
static const USBDescriptor hid_configuration_descriptor = {sizeof hid_configuration_descriptor_data, hid_configuration_descriptor_data};

/*
 * U.S. English language identifier
 */
static const uint8_t vcom_string0[] = {
    USB_DESC_BYTE(4),                     /* bLength.                         */
    USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
    USB_DESC_WORD(0x0409)                 /* wLANGID (U.S. English).          */
};

/*
 * Vendor string
 */
static const uint8_t vcom_string1[] = {USB_DESC_BYTE(38),                    /* bLength.                         */
                                       USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
                                       'S',
                                       0,
                                       'T',
                                       0,
                                       'M',
                                       0,
                                       '3',
                                       0,
                                       '2',
                                       0,
                                       'F',
                                       0,
                                       '4',
                                       0,
                                       '1',
                                       0,
                                       '1',
                                       0,
                                       ' ',
                                       0,
                                       'B',
                                       0,
                                       'l',
                                       0,
                                       'a',
                                       0,
                                       'c',
                                       0,
                                       'k',
                                       0,
                                       'P',
                                       0,
                                       'i',
                                       0,
                                       'l',
                                       0,
                                       'l',
                                       0};

/*
 * Device Description string
 */
static const uint8_t vcom_string2[] = {USB_DESC_BYTE(28),                    /* bLength.                         */
                                       USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
                                       'H',
                                       0,
                                       'I',
                                       0,
                                       'D',
                                       0,
                                       ' ',
                                       0,
                                       'K',
                                       0,
                                       'e',
                                       0,
                                       'y',
                                       0,
                                       'b',
                                       0,
                                       'o',
                                       0,
                                       'a',
                                       0,
                                       'r',
                                       0,
                                       'd',
                                       0,
                                       ' ',
                                       0};

/*
 * Serial Number string
 */
static const uint8_t vcom_string3[] = {USB_DESC_BYTE(8),                     /* bLength.                         */
                                       USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
                                       '0' + CH_KERNEL_MAJOR,
                                       0,
                                       '0' + CH_KERNEL_MINOR,
                                       0,
                                       '0' + CH_KERNEL_PATCH,
                                       0};

/*
 * Strings wrappers array
 */
static const USBDescriptor vcom_strings[] = {{sizeof vcom_string0, vcom_string0}, {sizeof vcom_string1, vcom_string1}, {sizeof vcom_string2, vcom_string2}, {sizeof vcom_string3, vcom_string3}};

/*
 * Handles the GET_DESCRIPTOR callback. All required descriptors must be
 * handled here.
 */
static const USBDescriptor *get_descriptor(USBDriver *usbp, uint8_t dtype, uint8_t dindex, uint16_t lang) {
    (void)usbp;
    (void)lang;
    switch (dtype) {
        case USB_DESCRIPTOR_DEVICE:
            return &vcom_device_descriptor;
        case USB_DESCRIPTOR_CONFIGURATION:
            return &hid_configuration_descriptor;
        case USB_DESCRIPTOR_STRING:
            if (dindex < 4) return &vcom_strings[dindex];
            break;
        case 0x22: /* HID Report Descriptor */
            return &hid_report_descriptor;
    }
    return NULL;
}

/**
 * @brief   IN EP1 state.
 */
static USBInEndpointState ep1instate;

/**
 * @brief   EP1 initialization structure (IN only).
 *          Endpoint size is 64 bytes (sufficient for 21-byte NKRO reports)
 */
static const USBEndpointConfig ep1config = {USB_EP_MODE_TYPE_INTR,
                                            NULL,
                                            NULL,
                                            NULL,
                                            0x0040, /* 64 bytes - accommodates 21-byte NKRO reports */
                                            0x0000,
                                            &ep1instate,
                                            NULL,
                                            1,
                                            NULL};

/*
 * Handles the USB driver global events
 */
static void usb_event(USBDriver *usbp, usbevent_t event) {
    switch (event) {
        case USB_EVENT_ADDRESS:
            return;
        case USB_EVENT_CONFIGURED:
            chSysLockFromISR();

            /* Enables the endpoints specified into the configuration.
               Note, this callback is invoked from an ISR so I-Class functions
               must be used.*/
            usbInitEndpointI(usbp, 1, &ep1config);

            chSysUnlockFromISR();
            return;
        case USB_EVENT_UNCONFIGURED:
            return;
        case USB_EVENT_RESET:
            /* Clear suspend flag on reset */
            usb_suspended = false;
            return;
        case USB_EVENT_SUSPEND:
            chSysLockFromISR();

            /* Set suspend flag - threads will enter low power mode */
            usb_suspended = true;

            chSysUnlockFromISR();
            return;
        case USB_EVENT_WAKEUP:
            chSysLockFromISR();

            /* Clear suspend flag - resume normal operation */
            usb_suspended = false;

            chSysUnlockFromISR();
            return;
        case USB_EVENT_STALLED:
            return;
    }
    return;
}

/*
 * Handles the USB driver requests.
 */
static bool requests_hook(USBDriver *usbp) {
    const USBDescriptor *dp;

    /* Check for HID class requests */
    if ((usbp->setup[0] & USB_RTYPE_TYPE_MASK) == USB_RTYPE_TYPE_CLASS) {
        switch (usbp->setup[1]) {
            case 0x01: /* GET_REPORT */
                return TRUE;
            case 0x09: /* SET_REPORT */
                return TRUE;
            case 0x0A: /* SET_IDLE */
                usbSetupTransfer(usbp, NULL, 0, NULL);
                return TRUE;
            case 0x0B: /* GET_PROTOCOL */
                return TRUE;
            default:
                return FALSE;
        }
    }

    /* Check for GET_DESCRIPTOR with HID descriptor type */
    if ((usbp->setup[0] & (USB_RTYPE_DIR_MASK | USB_RTYPE_TYPE_MASK)) == (USB_RTYPE_DIR_DEV2HOST | USB_RTYPE_TYPE_STD)) {
        if (usbp->setup[1] == USB_REQ_GET_DESCRIPTOR) {
            uint8_t dtype = usbp->setup[3];
            if (dtype == 0x22) { /* HID Report Descriptor */
                dp = &hid_report_descriptor;
                usbSetupTransfer(usbp, (uint8_t *)dp->ud_string, dp->ud_size, NULL);
                return TRUE;
            }
        }
    }

    return FALSE;
}

/*
 * USB driver configuration
 */
const USBConfig usbcfg = {usb_event, get_descriptor, requests_hook, NULL};

/*
 * Initialize USB
 */
void usb_start(void) {
    /*
     * Activates the USB driver and then the USB bus pull-up on D+.
     * Note, a delay is inserted in order to not have to disconnect the cable
     * after a reset.
     */
    usbDisconnectBus(&USBD1);
    chThdSleepMilliseconds(1500);
    usbStart(&USBD1, &usbcfg);
    usbConnectBus(&USBD1);
}

/*
 * Trigger USB remote wakeup
 * Call this to wake up a suspended USB host
 */
void usb_remote_wakeup(void) {
    if (usb_suspended && USBD1.state == USB_SUSPENDED) {
        usbWakeupHost(&USBD1);
    }
}
