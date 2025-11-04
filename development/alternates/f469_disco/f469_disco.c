// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

void matrix_init_pins(void) {
    // This pin is pulled low by default, but the normal direct pin init initialises each as input high.
    gpio_set_pin_input(A0);
}

#include "hal.h"

// Forward declarations
void SDRAM_Init(void);
void SDRAM_SanityTest(void);
void SDRAM_RunTests(void);

bool dsi_test_read_panel_id(void);

void keyboard_pre_init_kb() {
    keyboard_pre_init_user();
}

void housekeeping_task_kb(void) {
    // Call the user-defined housekeeping task
    housekeeping_task_user();

    static bool has_tested_ram = false;
    if (timer_read32() > 4000 && !has_tested_ram) {
        has_tested_ram = true;
        SDRAM_Init();
        SDRAM_SanityTest();
        SDRAM_RunTests();
        dsi_test_read_panel_id();
    }
}

#include "hal.h"
#include <stdint.h>
#include <stdbool.h>

// SDRAM configuration for STM32F469I-Discovery
#define SDRAM_BASE 0xC0000000
#define SDRAM_SIZE (16 * 1024 * 1024) // 16MB

void SDRAM_Init(void) {
    // 1. Enable clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOFEN | RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIOHEN | RCC_AHB1ENR_GPIOIEN;
    RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;

    // 2. Configure GPIO pins
    // clang-format off
    static const struct {
        stm32_gpio_t *port;
        uint8_t       pad;
    } sdram_pins[] = {
        // Address lines A0-A11
        {GPIOF, 0},  {GPIOF, 1},  {GPIOF, 2},  {GPIOF, 3},
        {GPIOF, 4},  {GPIOF, 5},  {GPIOF, 12}, {GPIOF, 13},
        {GPIOF, 14}, {GPIOF, 15}, {GPIOG, 0},  {GPIOG, 1},
        // Bank address BA0-BA1
        {GPIOG, 4}, {GPIOG, 5},
        // Data lines D0-D31
        {GPIOD, 14}, {GPIOD, 15}, {GPIOD, 0},  {GPIOD, 1},
        {GPIOE, 7},  {GPIOE, 8},  {GPIOE, 9},  {GPIOE, 10},
        {GPIOE, 11}, {GPIOE, 12}, {GPIOE, 13}, {GPIOE, 14},
        {GPIOE, 15}, {GPIOD, 8},  {GPIOD, 9},  {GPIOD, 10},
        {GPIOH, 8},  {GPIOH, 9},  {GPIOH, 10}, {GPIOH, 11},
        {GPIOH, 12}, {GPIOH, 13}, {GPIOH, 14}, {GPIOH, 15},
        {GPIOI, 0},  {GPIOI, 1},  {GPIOI, 2},  {GPIOI, 3},
        {GPIOI, 6},  {GPIOI, 7},  {GPIOI, 9},  {GPIOI, 10},
        // Control signals (SDNE0, SDCKE0, SDNRAS, SDNCAS, SDCLK, SDNWE)
        {GPIOH, 3},  {GPIOH, 2},  {GPIOF, 11}, {GPIOG, 15},
        {GPIOG, 8},  {GPIOC, 0},
        // Byte masks
        {GPIOE, 0},  {GPIOE, 1},  {GPIOI, 4},  {GPIOI, 5}
    };
    // clang-format on

    for (size_t i = 0; i < sizeof(sdram_pins) / sizeof(sdram_pins[0]); i++) {
        palSetPadMode(sdram_pins[i].port, sdram_pins[i].pad, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST);
    }

    // 3. FMC SDRAM Control Register
    uint32_t sdcr_config = (1 << FMC_SDCR1_RPIPE_Pos) |  // RPIPE: 1 HCLK clock cycle delay
                           (1 << FMC_SDCR1_RBURST_Pos) | // RBURST: Read burst enabled
                           (2 << FMC_SDCR1_SDCLK_Pos) |  // SDCLK: HCLK/2 = 90MHz
                           (0 << FMC_SDCR1_WP_Pos) |     // WP: Write protection disabled
                           (3 << FMC_SDCR1_CAS_Pos) |    // CAS: 3 cycles
                           (1 << FMC_SDCR1_NB_Pos) |     // NB: 4 internal banks
                           (2 << FMC_SDCR1_MWID_Pos) |   // MWID: 32-bit data bus
                           (1 << FMC_SDCR1_NR_Pos) |     // NR: 12-bit row address
                           (0 << FMC_SDCR1_NC_Pos);      // NC: 8-bit column address

    FMC_Bank5_6->SDCR[0] = sdcr_config;
    FMC_Bank5_6->SDCR[1] = sdcr_config;

    // 4. FMC SDRAM Timing Register
    uint32_t sdtr_config = ((2 - 1) << FMC_SDTR1_TRCD_Pos) | // TRCD: 2 cycles (min 18ns at 90MHz)
                           ((2 - 1) << FMC_SDTR1_TRP_Pos) |  // TRP: 2 cycles (min 18ns)
                           ((2 - 1) << FMC_SDTR1_TWR_Pos) |  // TWR: 2 cycles (min 2 clocks)
                           ((7 - 1) << FMC_SDTR1_TRC_Pos) |  // TRC: 7 cycles (min 60ns)
                           ((4 - 1) << FMC_SDTR1_TRAS_Pos) | // TRAS: 4 cycles (min 42ns)
                           ((7 - 1) << FMC_SDTR1_TXSR_Pos) | // TXSR: 7 cycles (min 70ns)
                           ((2 - 1) << FMC_SDTR1_TMRD_Pos);  // TMRD: 2 cycles (min 2 clocks)

    FMC_Bank5_6->SDTR[0] = sdtr_config;
    FMC_Bank5_6->SDTR[1] = sdtr_config;

    // 5. Initialization sequence
    volatile uint32_t timeout;

    // Step 1: Clock Configuration Enable
    FMC_Bank5_6->SDCMR = (1 << FMC_SDCMR_CTB1_Pos) | // CTB1: Target Bank 1
                         (1 << FMC_SDCMR_MODE_Pos);  // MODE: Clock Config Enable
    timeout = 0xFFFF;
    while ((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) && timeout--)
        ;

    // Wait 100μs for SDRAM power-up
    for (volatile int i = 0; i < 10000; i++)
        ;

    // Step 2: Precharge All Command
    FMC_Bank5_6->SDCMR = (1 << FMC_SDCMR_CTB1_Pos) | // CTB1: Target Bank 1
                         (2 << FMC_SDCMR_MODE_Pos);  // MODE: PALL
    timeout = 0xFFFF;
    while ((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) && timeout--)
        ;

    // Step 3: Auto-Refresh Command (8 cycles)
    FMC_Bank5_6->SDCMR = (7 << FMC_SDCMR_NRFS_Pos) | // NRFS: Number of auto-refresh: 8
                         (1 << FMC_SDCMR_CTB1_Pos) | // CTB1: Target Bank 1
                         (3 << FMC_SDCMR_MODE_Pos);  // MODE: Auto-refresh
    timeout = 0xFFFF;
    while ((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) && timeout--)
        ;

    // Step 4: Load Mode Register
    // SDRAM Mode Register format (from IS42S32400F datasheet):
    //   Bits [2:0] = Burst Length: 000 = 1 word
    //   Bit  [3]   = Burst Type: 0 = Sequential
    //   Bits [6:4] = CAS Latency: 011 = 3 cycles
    //   Bits [8:7] = Operating Mode: 00 = Standard
    //   Bit  [9]   = Write Burst Mode: 0 = Programmed burst length
    static const uint32_t SDRAM_MODE_BURST_LENGTH_1          = 0x0000;
    static const uint32_t SDRAM_MODE_BURST_TYPE_SEQUENTIAL   = 0x0000;
    static const uint32_t SDRAM_MODE_CAS_LATENCY_3           = 0x0030;
    static const uint32_t SDRAM_MODE_OPERATING_MODE_STANDARD = 0x0000;
    static const uint32_t SDRAM_MODE_WRITEBURST_MODE_SINGLE  = 0x0000;

    uint32_t mode_register = SDRAM_MODE_BURST_LENGTH_1 | SDRAM_MODE_BURST_TYPE_SEQUENTIAL | SDRAM_MODE_CAS_LATENCY_3 | SDRAM_MODE_OPERATING_MODE_STANDARD | SDRAM_MODE_WRITEBURST_MODE_SINGLE;

    FMC_Bank5_6->SDCMR = (mode_register << FMC_SDCMR_MRD_Pos) | // MRD: Mode register value
                         (1 << FMC_SDCMR_CTB1_Pos) |            // CTB1: Target Bank 1
                         (4 << FMC_SDCMR_MODE_Pos);             // MODE: Load Mode Register
    timeout = 0xFFFF;
    while ((FMC_Bank5_6->SDSR & FMC_SDSR_BUSY) && timeout--)
        ;

    // Step 5: Set Refresh Rate
    // Refresh period = 64ms / 4096 rows = 15.625μs per row
    // At 90MHz SDCLK: 15.625μs * 90MHz = 1406.25 clocks
    // Subtract 20 for safety margin: 1406 - 20 = 1386
    uint32_t refresh_count = 1386;
    FMC_Bank5_6->SDRTR     = (refresh_count << FMC_SDRTR_COUNT_Pos);

    // Stabilization delay
    for (volatile int i = 0; i < 1000; i++)
        ;
}

void SDRAM_SanityTest(void) {
    volatile uint32_t *sdram  = (volatile uint32_t *)SDRAM_BASE;
    int                errors = 0;

    dprintf("\n=== SDRAM Sanity Test ===\n");
    chThdSleepMilliseconds(50);

    // Test 1: Basic read/write
    dprintf("Test 1: Basic read/write\n");
    chThdSleepMilliseconds(50);

    sdram[100]   = 0x12345678;
    uint32_t val = sdram[100];
    dprintf("  Wrote: 0x12345678, Read: 0x%08X %s\n", (int)val, (val == 0x12345678) ? "PASS" : "FAIL");
    if (val != 0x12345678) errors++;
    chThdSleepMilliseconds(50);

    // Test 2: All 32 data bits
    dprintf("Test 2: 32-bit data bus\n");
    chThdSleepMilliseconds(50);

    int bit_errors = 0;
    for (int bit = 0; bit < 32; bit++) {
        uint32_t pattern   = 1U << bit;
        sdram[10000 + bit] = pattern;
        uint32_t readback  = sdram[10000 + bit];
        if (readback != pattern) bit_errors++;
    }
    dprintf("  All bits: %s\n", (bit_errors == 0) ? "PASS" : "FAIL");
    if (bit_errors > 0) errors += bit_errors;
    chThdSleepMilliseconds(50);

    // Test 3: Pattern fill (1MB)
    dprintf("Test 3: Sequential write/read (1MB)\n");
    chThdSleepMilliseconds(50);

    uint32_t test_size = 262144; // 1MB

    for (uint32_t i = 0; i < test_size; i++) {
        sdram[i] = 0xC0DE0000 + i;
    }

    int pattern_errors = 0;
    for (uint32_t i = 0; i < test_size; i++) {
        uint32_t expected = 0xC0DE0000 + i;
        if (sdram[i] != expected) {
            pattern_errors++;
            if (pattern_errors == 1) {
                dprintf("  First error at offset %d\n", (int)i);
            }
        }
    }

    if (pattern_errors == 0) {
        dprintf("  Sequential test: PASS (262144 words verified)\n");
    } else {
        dprintf("  Sequential test: FAIL (%d errors)\n", pattern_errors);
        errors += pattern_errors;
    }
    chThdSleepMilliseconds(50);

    // Test 4: Test full range
    dprintf("Test 4: Full 16MB range\n");
    chThdSleepMilliseconds(50);

    const uint32_t test_offsets[] = {0, 1048576, 2097152, 3145728, 4194303}; // 0MB, 4MB, 8MB, 12MB, ~16MB
    const char    *labels[]       = {"0MB", "4MB", "8MB", "12MB", "~16MB"};

    for (int i = 0; i < 5; i++) {
        uint32_t pattern       = 0xF0000000 + i;
        sdram[test_offsets[i]] = pattern;
        uint32_t readback      = sdram[test_offsets[i]];
        dprintf("  %s: %s\n", labels[i], (readback == pattern) ? "PASS" : "FAIL");
        if (readback != pattern) errors++;
        chThdSleepMilliseconds(30);
    }

    // Summary
    dprintf("\n=== Test Summary ===\n");
    chThdSleepMilliseconds(50);
    if (errors == 0) {
        dprintf("ALL TESTS PASSED!\n");
        dprintf("16MB SDRAM at 0x%08X is fully functional.\n", SDRAM_BASE);
    } else {
        dprintf("TESTS FAILED - %d total errors\n", errors);
    }
    chThdSleepMilliseconds(50);
}

#include "hal.h"
#include <stdint.h>
#include <stdbool.h>

#define SDRAM_BASE 0xC0000000
#define SDRAM_SIZE (16 * 1024 * 1024) // 16MB

// Simple LCG PRNG for deterministic testing
typedef struct {
    uint32_t state;
} prng_t;

static prng_t g_prng;

static void prng_seed(prng_t *prng, uint32_t seed) {
    prng->state = seed;
}

static uint32_t prng_next(prng_t *prng) {
    // LCG parameters from Numerical Recipes
    prng->state = prng->state * 1664525UL + 1013904223UL;
    return prng->state;
}

// Test result structure
typedef struct {
    bool     passed;
    uint32_t errors;
    uint32_t first_error_addr;
    uint32_t expected_value;
    uint32_t actual_value;
} sdram_test_result_t;

// Test entire SDRAM with pseudorandom data
sdram_test_result_t SDRAM_TestFull(uint32_t seed) {
    sdram_test_result_t result = {.passed = true, .errors = 0, .first_error_addr = 0, .expected_value = 0, .actual_value = 0};

    volatile uint32_t *sdram      = (volatile uint32_t *)SDRAM_BASE;
    uint32_t           word_count = SDRAM_SIZE / sizeof(uint32_t); // 4M words

    dprintf("\n=== Full SDRAM Test (16MB) ===\n");
    chThdSleepMilliseconds(50);
    dprintf("Testing %d words with seed 0x%08X\n", (int)word_count, (int)seed);
    chThdSleepMilliseconds(50);

    // Write phase
    dprintf("Writing...\n");
    chThdSleepMilliseconds(50);

    prng_seed(&g_prng, seed);
    for (uint32_t i = 0; i < word_count; i++) {
        sdram[i] = prng_next(&g_prng);

        // Progress updates and yielding
        if ((i & 0xFFFF) == 0) {         // Every 64K words
            chThdSleepMicroseconds(100); // Yield
        }
        if ((i & 0xFFFFF) == 0) { // Every 1M words
            int percent = (int)((i * 100) / word_count);
            dprintf("  Written: %d%%\n", percent);
            chThdSleepMilliseconds(10);
        }
    }
    dprintf("  Written: 100%%\n");
    chThdSleepMilliseconds(50);

    // Verify phase
    dprintf("Verifying...\n");
    chThdSleepMilliseconds(50);

    prng_seed(&g_prng, seed); // Reset PRNG to same seed
    for (uint32_t i = 0; i < word_count; i++) {
        uint32_t expected = prng_next(&g_prng);
        uint32_t actual   = sdram[i];

        if (actual != expected) {
            if (result.errors == 0) {
                // Record first error
                result.first_error_addr = (uint32_t)&sdram[i];
                result.expected_value   = expected;
                result.actual_value     = actual;
            }
            result.errors++;
            result.passed = false;

            // Stop after finding too many errors
            if (result.errors >= 100) {
                dprintf("  Stopping after 100 errors...\n");
                chThdSleepMilliseconds(50);
                break;
            }
        }

        // Progress updates and yielding
        if ((i & 0xFFFF) == 0) {
            chThdSleepMicroseconds(100);
        }
        if ((i & 0xFFFFF) == 0) {
            int percent = (int)((i * 100) / word_count);
            dprintf("  Verified: %d%%\n", percent);
            chThdSleepMilliseconds(10);
        }
    }

    if (result.errors < 100) {
        dprintf("  Verified: 100%%\n");
        chThdSleepMilliseconds(50);
    }

    return result;
}

// Quick test - test every Nth word for faster validation
sdram_test_result_t SDRAM_TestQuick(uint32_t seed, uint32_t stride) {
    sdram_test_result_t result = {.passed = true, .errors = 0, .first_error_addr = 0, .expected_value = 0, .actual_value = 0};

    volatile uint32_t *sdram      = (volatile uint32_t *)SDRAM_BASE;
    uint32_t           word_count = SDRAM_SIZE / sizeof(uint32_t);

    dprintf("\n=== Quick SDRAM Test (stride=%d) ===\n", (int)stride);
    chThdSleepMilliseconds(50);

    // Write phase
    dprintf("Writing...\n");
    chThdSleepMilliseconds(50);

    prng_seed(&g_prng, seed);
    uint32_t test_count = 0;
    for (uint32_t i = 0; i < word_count; i += stride) {
        sdram[i] = prng_next(&g_prng);
        test_count++;

        if ((test_count & 0xFFF) == 0) {
            chThdSleepMicroseconds(100);
        }
    }
    dprintf("  Wrote %d words\n", (int)test_count);
    chThdSleepMilliseconds(50);

    // Verify phase
    dprintf("Verifying...\n");
    chThdSleepMilliseconds(50);

    prng_seed(&g_prng, seed);
    test_count = 0;
    for (uint32_t i = 0; i < word_count; i += stride) {
        uint32_t expected = prng_next(&g_prng);
        uint32_t actual   = sdram[i];

        if (actual != expected) {
            if (result.errors == 0) {
                result.first_error_addr = (uint32_t)&sdram[i];
                result.expected_value   = expected;
                result.actual_value     = actual;
            }
            result.errors++;
            result.passed = false;
        }

        test_count++;
        if ((test_count & 0xFFF) == 0) {
            chThdSleepMicroseconds(100);
        }
    }

    dprintf("  Verified %d words\n", (int)test_count);
    chThdSleepMilliseconds(50);

    return result;
}

// Run comprehensive tests
void SDRAM_RunTests(void) {
    sdram_test_result_t result;

    dprintf("\n=== SDRAM Comprehensive Tests ===\n");
    chThdSleepMilliseconds(50);

    // Test 1: Quick test with stride
    dprintf("\n--- Test 1: Quick Test (every 256th word) ---\n");
    chThdSleepMilliseconds(50);
    result = SDRAM_TestQuick(0x12345678, 256);

    if (result.passed) {
        dprintf("Quick test: PASS\n");
    } else {
        dprintf("Quick test: FAIL - %d errors\n", (int)result.errors);
        dprintf("  First error at 0x%08X\n", (int)result.first_error_addr);
        dprintf("  Expected: 0x%08X, Got: 0x%08X\n", (int)result.expected_value, (int)result.actual_value);
        return; // Don't continue if quick test fails
    }
    chThdSleepMilliseconds(50);

    // Test 2: Full test
    dprintf("\n--- Test 2: Full Test (all 4M words) ---\n");
    dprintf("This will take 2-3 minutes...\n");
    chThdSleepMilliseconds(100);

    result = SDRAM_TestFull(0xDEADBEEF);

    if (result.passed) {
        dprintf("\nFull test: PASS\n");
        dprintf("All 4,194,304 words verified successfully!\n");
    } else {
        dprintf("\nFull test: FAIL - %d errors detected\n", (int)result.errors);
        dprintf("  First error at 0x%08X\n", (int)result.first_error_addr);
        dprintf("  Expected: 0x%08X, Got: 0x%08X\n", (int)result.expected_value, (int)result.actual_value);
    }
    chThdSleepMilliseconds(50);

    // Test 3: Multiple passes with different seeds
    dprintf("\n--- Test 3: Multi-seed test ---\n");
    chThdSleepMilliseconds(50);

    uint32_t seeds[]    = {0xAAAAAAAA, 0x55555555, 0x00000000, 0xFFFFFFFF};
    bool     all_passed = true;

    for (int i = 0; i < 4; i++) {
        dprintf("  Seed 0x%08X: ", (int)seeds[i]);
        chThdSleepMilliseconds(50);

        result = SDRAM_TestQuick(seeds[i], 512);

        if (result.passed) {
            dprintf("PASS\n");
        } else {
            dprintf("FAIL (%d errors)\n", (int)result.errors);
            all_passed = false;
        }
        chThdSleepMilliseconds(50);
    }

    if (all_passed) {
        dprintf("Multi-seed test: PASS\n");
    } else {
        dprintf("Multi-seed test: FAIL\n");
    }
    chThdSleepMilliseconds(50);

    // Final summary
    dprintf("\n=== Test Complete ===\n");
    chThdSleepMilliseconds(50);
}

// Usage:
// SDRAM_Init();
// SDRAM_RunTests();
