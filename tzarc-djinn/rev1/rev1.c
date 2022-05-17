// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <hal.h>
#include <quantum.h>
#include "rev1.h"

#ifdef USE_PLUG_DETECT_PIN
bool is_keyboard_master(void) {
    static bool determined = false;
    static bool is_master;
    if (!determined) {
        determined = true;
        setPinInputLow(SPLIT_PLUG_DETECT_PIN);
        wait_ms(50);
        is_master = readPin(SPLIT_PLUG_DETECT_PIN) ? true : false;
        if (!is_master) {
            usbStop(&USBD1);
        }
    }

    return is_master;
}
#endif // USE_PLUG_DETECT_PIN

#ifdef RGB_MATRIX_ENABLE
// clang-format off
led_config_t g_led_config = {
    {
        // Key Matrix to LED Index
        {35    , 36    , 37    , 38    , 39    , 40    , 41    },
        {34    , 33    , 32    , 31    , 30    , 29    , 28    },
        {21    , 22    , 23    , 24    , 25    , 26    , 27    },
        {20    , 19    , 18    , 17    , 16    , 15    , 14    },
        {NO_LED, NO_LED, NO_LED, 10    , 11    , 12    , 13    },
        {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},
        {77    , 78    , 79    , 80    , 81    , 82    , 83    },
        {76    , 75    , 74    , 73    , 72    , 71    , 70    },
        {63    , 64    , 65    , 66    , 67    , 68    , 69    },
        {62    , 61    , 60    , 59    , 58    , 57    , 56    },
        {NO_LED, NO_LED, NO_LED, 52    , 53    , 54    , 55    },
        {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},
    },
    {
        // LED Index to Physical Position
        // Underglow left
        {110, 30 }, {110, 80 }, {110, 130}, {95 , 180}, {75 , 195}, {60 , 210}, {45 , 190}, {35 , 175}, {20 , 160}, {5  , 160},
        // Matrix left
        {78 , 150}, {104, 150}, {130, 150}, {156, 150},
        {156, 120}, {130, 120}, {104, 120}, {78 , 120}, {52 , 120}, {26 , 120}, {0  , 120},
        {0  , 90 }, {26 , 90 }, {52 , 90 }, {78 , 90 }, {104, 90 }, {130, 90 }, {156, 90 },
        {156, 60 }, {130, 60 }, {104, 60 }, {78 , 60 }, {52 , 60 }, {26 , 60 }, {0  , 60 },
        {0  , 30 }, {26 , 30 }, {52 , 30 }, {78 , 30 }, {104, 30 }, {130, 30 }, {156, 30 },
        // Underglow right
        {114, 30 }, {114, 80 }, {114, 130}, {129, 180}, {149, 195}, {164, 210}, {179, 190}, {189, 175}, {204, 160}, {219, 160},
        // Matrix right
        {146, 150}, {120, 150}, {94 , 150}, {68 , 150},
        {68 , 120}, {94 , 120}, {120, 120}, {146, 120}, {172, 120}, {198, 120}, {224, 120},
        {224, 90 }, {198, 90 }, {172, 90 }, {146, 90 }, {120, 90 }, {94 , 90 }, {68 , 90 },
        {68 , 60 }, {94 , 60 }, {120, 60 }, {146, 60 }, {172, 60 }, {198, 60 }, {224, 60 },
        {224, 30 }, {198, 30 }, {172, 30 }, {146, 30 }, {120, 30 }, {94 , 30 }, {68 , 30 },
    },
    {
        // LED Index to Flag
        // Underglow left
        LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW,
        // Matrix left
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        // Underglow right
        LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW, LED_FLAG_UNDERGLOW,
        // Matrix right
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
    }
};
// clang-format on
#endif // RGB_MATRIX_ENABLE
