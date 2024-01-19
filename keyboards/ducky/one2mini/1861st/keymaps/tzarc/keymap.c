// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

// Apparently rules.mk and config.h aren't picked up unless there's a keymap.c in this directory.
// Reroute to the layout instead.
#include "layouts/60_ansi/tzarc/keymap.c"
