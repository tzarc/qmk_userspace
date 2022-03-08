// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <quantum.h>

#ifdef RGBLIGHT_ENABLE
#    include <rgblight.h>
#endif

#include <raw_hid.h>
#include "tzarc.h"

#if defined(RAW_ENABLE) && !defined(VIA_ENABLE)

void raw_hid_receive(uint8_t *data, uint8_t length) {
    for (size_t i = 0; i < length; ++i)
        data[i] ^= 0xFF;

    // Loopback for now.
    raw_hid_send(data, length);
}

#endif