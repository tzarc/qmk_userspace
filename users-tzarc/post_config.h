// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// Disable old macros
#ifndef NO_ACTION_MACRO
#    define NO_ACTION_MACRO
#endif // NO_ACTION_MACRO

// Disable old functions
#ifndef NO_ACTION_FUNCTION
#    define NO_ACTION_FUNCTION
#endif // NO_ACTION_FUNCTION

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#ifdef LOCKING_SUPPORT_ENABLE
#    undef LOCKING_SUPPORT_ENABLE
#endif // LOCKING_SUPPORT_ENABLE

/* Locking resynchronize hack */
#ifdef LOCKING_RESYNC_ENABLE
#    undef LOCKING_RESYNC_ENABLE
#endif // LOCKING_RESYNC_ENABLE
