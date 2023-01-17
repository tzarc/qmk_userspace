// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include_next <chconf.h>

#undef CH_CFG_SYSTEM_HALT_HOOK
#define CH_CFG_SYSTEM_HALT_HOOK(reason) {                                   \
  /* System halt code here.*/                                               \
  extern void chibi_system_halt_hook(const char *);                         \
  chibi_system_halt_hook(reason);                                           \
}

#undef CH_CFG_TRACE_HOOK
#define CH_CFG_TRACE_HOOK(tep) {                                            \
  /* Trace code here.*/                                                     \
  extern void chibi_system_trace_hook(void *);                  \
  chibi_system_trace_hook(tep);                                             \
}
