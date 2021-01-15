#include <quantum.h>
#include <rgblight.h>
#include <raw_hid.h>
#include "tzarc.h"

#if defined(RAW_ENABLE) && !defined(VIA_ENABLE)

void raw_hid_receive(uint8_t *data, uint8_t length) {
    for (size_t i = 0; i < length; ++i) data[i] ^= 0xFF;

    // Loopback for now.
    raw_hid_send(data, length);
}

#endif