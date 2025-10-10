/*
    STM32F411 BlackPill USB HID Keyboard
    ChibiOS/RT Configuration
*/

#ifndef CHCONF_H
#define CHCONF_H

#define _CHIBIOS_RT_CONF_
#define _CHIBIOS_RT_CONF_VER_7_0_

/*===========================================================================*/
/**
 * @name System settings
 * @{
 */
/*===========================================================================*/

#if !defined(CH_CFG_SMP_MODE)
#define CH_CFG_SMP_MODE                     FALSE
#endif

/** @} */

/*===========================================================================*/
/**
 * @name System timers settings
 * @{
 */
/*===========================================================================*/

#if !defined(CH_CFG_ST_RESOLUTION)
#define CH_CFG_ST_RESOLUTION                32
#endif

#if !defined(CH_CFG_ST_FREQUENCY)
#define CH_CFG_ST_FREQUENCY                 10000
#endif

#if !defined(CH_CFG_INTERVALS_SIZE)
#define CH_CFG_INTERVALS_SIZE               32
#endif

#if !defined(CH_CFG_TIME_TYPES_SIZE)
#define CH_CFG_TIME_TYPES_SIZE              32
#endif

#if !defined(CH_CFG_ST_TIMEDELTA)
#define CH_CFG_ST_TIMEDELTA                 2
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Kernel parameters and options
 * @{
 */
/*===========================================================================*/

#if !defined(CH_CFG_TIME_QUANTUM)
#define CH_CFG_TIME_QUANTUM                 0
#endif

#if !defined(CH_CFG_NO_IDLE_THREAD)
#define CH_CFG_NO_IDLE_THREAD               FALSE
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Performance options
 * @{
 */
/*===========================================================================*/

#if !defined(CH_CFG_OPTIMIZE_SPEED)
#define CH_CFG_OPTIMIZE_SPEED               TRUE
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Subsystem options
 * @{
 */
/*===========================================================================*/

#if !defined(CH_CFG_USE_TM)
#define CH_CFG_USE_TM                       TRUE
#endif

#if !defined(CH_CFG_USE_TIMESTAMP)
#define CH_CFG_USE_TIMESTAMP                TRUE
#endif

#if !defined(CH_CFG_USE_REGISTRY)
#define CH_CFG_USE_REGISTRY                 TRUE
#endif

#if !defined(CH_CFG_USE_WAITEXIT)
#define CH_CFG_USE_WAITEXIT                 TRUE
#endif

#if !defined(CH_CFG_USE_SEMAPHORES)
#define CH_CFG_USE_SEMAPHORES               TRUE
#endif

#if !defined(CH_CFG_USE_SEMAPHORES_PRIORITY)
#define CH_CFG_USE_SEMAPHORES_PRIORITY      FALSE
#endif

#if !defined(CH_CFG_USE_MUTEXES)
#define CH_CFG_USE_MUTEXES                  TRUE
#endif

#if !defined(CH_CFG_USE_MUTEXES_RECURSIVE)
#define CH_CFG_USE_MUTEXES_RECURSIVE        FALSE
#endif

#if !defined(CH_CFG_USE_CONDVARS)
#define CH_CFG_USE_CONDVARS                 TRUE
#endif

#if !defined(CH_CFG_USE_CONDVARS_TIMEOUT)
#define CH_CFG_USE_CONDVARS_TIMEOUT         TRUE
#endif

#if !defined(CH_CFG_USE_EVENTS)
#define CH_CFG_USE_EVENTS                   TRUE
#endif

#if !defined(CH_CFG_USE_EVENTS_TIMEOUT)
#define CH_CFG_USE_EVENTS_TIMEOUT           TRUE
#endif

#if !defined(CH_CFG_USE_MESSAGES)
#define CH_CFG_USE_MESSAGES                 TRUE
#endif

#if !defined(CH_CFG_USE_MESSAGES_PRIORITY)
#define CH_CFG_USE_MESSAGES_PRIORITY        FALSE
#endif

#if !defined(CH_CFG_USE_DYNAMIC)
#define CH_CFG_USE_DYNAMIC                  TRUE
#endif

/** @} */

/*===========================================================================*/
/**
 * @name OSLIB options
 * @{
 */
/*===========================================================================*/

#if !defined(CH_CFG_USE_MAILBOXES)
#define CH_CFG_USE_MAILBOXES                TRUE
#endif

#if !defined(CH_CFG_USE_MEMCORE)
#define CH_CFG_USE_MEMCORE                  TRUE
#endif

#if !defined(CH_CFG_MEMCORE_SIZE)
#define CH_CFG_MEMCORE_SIZE                 0
#endif

#if !defined(CH_CFG_USE_HEAP)
#define CH_CFG_USE_HEAP                     TRUE
#endif

#if !defined(CH_CFG_USE_MEMPOOLS)
#define CH_CFG_USE_MEMPOOLS                 TRUE
#endif

#if !defined(CH_CFG_USE_OBJ_FIFOS)
#define CH_CFG_USE_OBJ_FIFOS                TRUE
#endif

#if !defined(CH_CFG_USE_PIPES)
#define CH_CFG_USE_PIPES                    TRUE
#endif

#if !defined(CH_CFG_USE_OBJ_CACHES)
#define CH_CFG_USE_OBJ_CACHES               TRUE
#endif

#if !defined(CH_CFG_USE_DELEGATES)
#define CH_CFG_USE_DELEGATES                TRUE
#endif

#if !defined(CH_CFG_USE_JOBS)
#define CH_CFG_USE_JOBS                     TRUE
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Objects factory options
 * @{
 */
/*===========================================================================*/

#if !defined(CH_CFG_USE_FACTORY)
#define CH_CFG_USE_FACTORY                  TRUE
#endif

#if !defined(CH_CFG_FACTORY_MAX_NAMES_LENGTH)
#define CH_CFG_FACTORY_MAX_NAMES_LENGTH     8
#endif

#if !defined(CH_CFG_FACTORY_OBJECTS_REGISTRY)
#define CH_CFG_FACTORY_OBJECTS_REGISTRY     TRUE
#endif

#if !defined(CH_CFG_FACTORY_GENERIC_BUFFERS)
#define CH_CFG_FACTORY_GENERIC_BUFFERS      TRUE
#endif

#if !defined(CH_CFG_FACTORY_SEMAPHORES)
#define CH_CFG_FACTORY_SEMAPHORES           TRUE
#endif

#if !defined(CH_CFG_FACTORY_MAILBOXES)
#define CH_CFG_FACTORY_MAILBOXES            TRUE
#endif

#if !defined(CH_CFG_FACTORY_OBJ_FIFOS)
#define CH_CFG_FACTORY_OBJ_FIFOS            TRUE
#endif

#if !defined(CH_CFG_FACTORY_PIPES) || defined(__DOXYGEN__)
#define CH_CFG_FACTORY_PIPES                TRUE
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Debug options
 * @{
 */
/*===========================================================================*/

#if !defined(CH_DBG_STATISTICS)
#define CH_DBG_STATISTICS                   FALSE
#endif

#if !defined(CH_DBG_SYSTEM_STATE_CHECK)
#define CH_DBG_SYSTEM_STATE_CHECK           FALSE
#endif

#if !defined(CH_DBG_ENABLE_CHECKS)
#define CH_DBG_ENABLE_CHECKS                FALSE
#endif

#if !defined(CH_DBG_ENABLE_ASSERTS)
#define CH_DBG_ENABLE_ASSERTS               FALSE
#endif

#if !defined(CH_DBG_TRACE_MASK)
#define CH_DBG_TRACE_MASK                   CH_DBG_TRACE_MASK_DISABLED
#endif

#if !defined(CH_DBG_TRACE_BUFFER_SIZE)
#define CH_DBG_TRACE_BUFFER_SIZE            128
#endif

#if !defined(CH_DBG_ENABLE_STACK_CHECK)
#define CH_DBG_ENABLE_STACK_CHECK           FALSE
#endif

#if !defined(CH_DBG_FILL_THREADS)
#define CH_DBG_FILL_THREADS                 FALSE
#endif

#if !defined(CH_DBG_THREADS_PROFILING)
#define CH_DBG_THREADS_PROFILING            FALSE
#endif

/** @} */

/*===========================================================================*/
/**
 * @name Kernel hooks
 * @{
 */
/*===========================================================================*/

#define CH_CFG_SYSTEM_EXTRA_FIELDS

#define CH_CFG_SYSTEM_INIT_HOOK() {                                         \
}

#define CH_CFG_OS_INSTANCE_EXTRA_FIELDS

#define CH_CFG_OS_INSTANCE_INIT_HOOK(oip) {                                 \
}

#define CH_CFG_THREAD_EXTRA_FIELDS

#define CH_CFG_THREAD_INIT_HOOK(tp) {                                       \
}

#define CH_CFG_THREAD_EXIT_HOOK(tp) {                                       \
}

#define CH_CFG_CONTEXT_SWITCH_HOOK(ntp, otp) {                              \
}

#define CH_CFG_IRQ_PROLOGUE_HOOK() {                                        \
}

#define CH_CFG_IRQ_EPILOGUE_HOOK() {                                        \
}

#define CH_CFG_IDLE_ENTER_HOOK() {                                          \
}

#define CH_CFG_IDLE_LEAVE_HOOK() {                                          \
}

#define CH_CFG_IDLE_LOOP_HOOK() {                                           \
}

#define CH_CFG_SYSTEM_TICK_HOOK() {                                         \
}

#define CH_CFG_SYSTEM_HALT_HOOK(reason) {                                   \
}

#define CH_CFG_TRACE_HOOK(tep) {                                            \
}

#define CH_CFG_RUNTIME_FAULTS_HOOK(mask) {                                  \
}

/** @} */

#endif  /* CHCONF_H */
