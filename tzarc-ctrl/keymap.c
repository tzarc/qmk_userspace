// Apparently rules.mk and config.h aren't picked up unless there's a keymap.c in this directory.

// Total hack to get the same damn layout, why isn't it a LAYOUT_tkl_ansi?
#define LAYOUT_tkl_ansi(...) LAYOUT(__VA_ARGS__)

// Reroute to the layout instead.
#include "layouts/community/tkl_ansi/tzarc/keymap.c"
