// Copyright 2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

//#define CH_CFG_ST_TIMEDELTA 0
#define CH_CFG_ST_RESOLUTION 16
#define CH_CFG_ST_FREQUENCY 10000
//#define CH_CFG_INTERVALS_SIZE 64

//#define CH_CFG_USE_TM                       FALSE
//#define CH_DBG_STATISTICS                   FALSE
#define CH_DBG_SYSTEM_STATE_CHECK           TRUE
#define CH_DBG_ENABLE_CHECKS                TRUE
#define CH_DBG_ENABLE_ASSERTS               TRUE
//#define CH_DBG_TRACE_MASK                   CH_DBG_TRACE_MASK_ALL
//#define CH_DBG_TRACE_BUFFER_SIZE            512
#define CH_DBG_ENABLE_STACK_CHECK           TRUE

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
