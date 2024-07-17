// Copyright 2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <string.h>
#include <stdbool.h>
#include "quantum.h"
#include "debug.h"
#include "rgb_matrix.h"
#include "rv32_rgb_runner.inl.h"
#include "rv32_rgb_runner/rv32_runner.h"

#ifndef RGB_MATRIX_RV32_RUNNER_RAM
#    define RGB_MATRIX_RV32_RUNNER_RAM 1024
#endif // RGB_MATRIX_RV32_RUNNER_RAM

#define MINI_RV32_RAM_SIZE (RGB_MATRIX_RV32_RUNNER_RAM)

// This needs to match the RAM address for the chip in use, STM32 seem to be 0x20000000 onwards, with 0x11100000 used as SYSCON-like support
#define MINIRV32_MMIO_RANGE(n) ((0x20000000 <= (n) && (n) < (0x20000000 + (512 * 1024))) || (n) == 0x11100000)

#define MINIRV32_HANDLE_MEM_LOAD_CONTROL(addy, rval) \
    do {                                             \
        rval = rv32vm_handle_load(addy);             \
    } while (0)

static uint32_t rv32vm_handle_load(uint32_t addy);

#define MINIRV32_HANDLE_MEM_STORE_CONTROL(addy, val)    \
    do {                                                \
        if (rv32vm_handle_store(addy, val)) return val; \
    } while (0)

static uint32_t rv32vm_handle_store(uint32_t addy, uint32_t val);

#define MINIRV32_STEPPROTO MINIRV32_DECORATE int32_t MiniRV32IMAStepRGB(struct MiniRV32IMAState* state, uint8_t* image, uint32_t vProcAddress, uint32_t elapsedUs, int count)

#define MINIRV32_IMPLEMENTATION
#include "mini-rv32ima.h"

static uint8_t                 rgb_ram_area[MINI_RV32_RAM_SIZE];
static struct MiniRV32IMAState rgb_core;

static uint32_t rv32vm_handle_load(uint32_t addy) {
    return 0;
}

static uint32_t rv32vm_handle_store(uint32_t addy, uint32_t val) {
    if (addy == 0x11100000) {          // SYSCON
        rgb_core.pc = rgb_core.pc + 4; // resume executing at next opcode
        return val;
    }
    return 0;
}

static void rv32vm_vcall_handler(void) {
    switch (rgb_core.regs[10]) {
        case RV32_VCALL_TIMER_READ32:
            rgb_core.regs[10] = timer_read32();
            break;
        case RV32_VCALL_RGB_MATRIX_SET_COLOR:
            rgb_matrix_set_color(rgb_core.regs[11], rgb_core.regs[12], rgb_core.regs[13], rgb_core.regs[14]);
            break;
    }
}

static void rv32vm_invoke(rv32_api_t api) {
    rgb_core.pc = MINIRV32_RAM_IMAGE_OFFSET;
    rgb_core.extraflags |= 3; // Machine mode
    rgb_core.regs[5] = (uint32_t)api;

    int ret;
    do {
        ret = MiniRV32IMAStepRGB(&rgb_core, rgb_ram_area, 0, 0, 1024);
        switch (ret) {
            case 0:
                break;
            case RV32_VCALL:
                rv32vm_vcall_handler();
                ret = 0;
                break;
            case RV32_EXIT:
                return;
        }
    } while (ret == 0);
}

void rv32vm_effect_init_impl(effect_params_t* params) {
    debug_enable = true;
    memset(&rgb_core, 0, sizeof(rgb_core));
    memset(rgb_ram_area, 0, sizeof(rgb_ram_area));
    memcpy(rgb_ram_area, rv32_runner, rv32_runner_len);
    rv32vm_invoke(RV32_EFFECT_INIT);
}

void rv32vm_effect_led_impl(effect_params_t* params, uint8_t led_index) {
    rgb_core.regs[10] = (uint32_t)(uintptr_t)params;
    rgb_core.regs[11] = (uint32_t)led_index;
    rv32vm_invoke(RV32_EFFECT_LED);
}
