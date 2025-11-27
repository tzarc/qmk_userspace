// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// Helper function to send DCS short write with 0 parameters
static void dsi_dcs_write_0param(uint8_t cmd) {
    // Wait for FIFO space
    uint32_t timeout = 1000;
    while ((DSI->GPSR & DSI_GPSR_CMDFF) && timeout > 0) {
        chThdSleepMicroseconds(10);
        timeout--;
    }

    if (timeout == 0) {
        dprintf("    [DCS 0x%02X] FIFO full before write\n", cmd);
    }

    // Write command
    DSI->GHCR = (0x05U) | (0U << 6) | (cmd << 8);

    // Wait for transmission
    timeout = 1000;
    while (!(DSI->GPSR & DSI_GPSR_CMDFE) && timeout > 0) {
        chThdSleepMicroseconds(100);
        timeout--;
    }

    if (timeout == 0) {
        dprintf("    [DCS 0x%02X] Command timeout\n", cmd);
    }
}

// Helper function to send DCS short write with 1 parameter
static void dsi_dcs_write_1param(uint8_t cmd, uint8_t param) {
    // Wait for FIFO space
    uint32_t timeout = 1000;
    while ((DSI->GPSR & DSI_GPSR_CMDFF) && timeout > 0) {
        chThdSleepMicroseconds(10);
        timeout--;
    }

    // Write command
    DSI->GHCR = (0x15U) | (0U << 6) | (cmd << 8) | (param << 16);

    // Wait for transmission
    timeout = 1000;
    while (!(DSI->GPSR & DSI_GPSR_CMDFE) && timeout > 0) {
        chThdSleepMicroseconds(100);
        timeout--;
    }
}

// Helper function to send Generic Long Write
static void dsi_generic_write(uint8_t cmd, const uint8_t *params, uint8_t len) {
    // Wait for FIFO space
    uint32_t timeout = 1000;
    while ((DSI->GPSR & DSI_GPSR_CMDFF) && timeout > 0) {
        chThdSleepMicroseconds(10);
        timeout--;
    }

    // First word: command + up to 3 params
    uint32_t data = cmd;
    for (int i = 0; i < len && i < 3; i++) {
        data |= (uint32_t)params[i] << (8 * (i + 1));
    }
    DSI->GPDR = data;

    // Remaining params
    for (int i = 3; i < len; i += 4) {
        data = 0;
        for (int j = 0; j < 4 && (i + j) < len; j++) {
            data |= (uint32_t)params[i + j] << (8 * j);
        }
        DSI->GPDR = data;
    }

    // Send long write header
    DSI->GHCR = (0x29U) | (0U << 6) | ((len + 1) << 8);

    // Wait for transmission
    timeout = 1000;
    while (!(DSI->GPSR & DSI_GPSR_CMDFE) && timeout > 0) {
        chThdSleepMicroseconds(100);
        timeout--;
    }
}

// Minimal OTM8009A initialization to enable CMD2 and allow ID reads
__attribute__((used)) static bool otm8009a_enable_cmd2(void) {
    dprintf("Enabling OTM8009A CMD2 mode...\n");

    // Check GPSR before we start
    dprintf("  Initial GPSR: 0x%08lX\n", (unsigned long)DSI->GPSR);

    // Step 1: Send NOP to shift address to 0x00
    dprintf("  Sending NOP (0x00)...\n");
    dsi_dcs_write_0param(0x00); // NOP
    chThdSleepMilliseconds(1);
    dprintf("    GPSR: 0x%08lX\n", (unsigned long)DSI->GPSR);

    // Step 2: Enable CMD2 - write 0xFF register with {0x80, 0x09, 0x01}
    dprintf("  Writing 0xFF = {0x80, 0x09, 0x01}...\n");
    const uint8_t cmd2_enable[] = {0x80, 0x09, 0x01};
    dsi_generic_write(0xFF, cmd2_enable, 3);
    chThdSleepMilliseconds(10);
    dprintf("    GPSR: 0x%08lX\n", (unsigned long)DSI->GPSR);

    // Step 3: Send NOP to shift address to 0x80
    dprintf("  Sending NOP with param 0x80...\n");
    dsi_dcs_write_1param(0x00, 0x80); // NOP with parameter 0x80
    chThdSleepMilliseconds(1);
    dprintf("    GPSR: 0x%08lX\n", (unsigned long)DSI->GPSR);

    // Step 4: Enter ORISE Command 2 - write 0xFF register with {0x80, 0x09}
    dprintf("  Writing 0xFF = {0x80, 0x09}...\n");
    const uint8_t orise_cmd2[] = {0x80, 0x09};
    dsi_generic_write(0xFF, orise_cmd2, 2);
    chThdSleepMilliseconds(10);
    dprintf("    GPSR: 0x%08lX\n", (unsigned long)DSI->GPSR);

    dprintf("CMD2 mode enabled\n");
    return true;
}

bool dsi_test_read_panel_id(void) {
    uint8_t  panel_id[3] = {0, 0, 0};
    uint32_t timeout;

    dprintf("Step 0: Configuring display GPIO pins...\n");

    // Enable GPIO clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN | RCC_AHB1ENR_GPIOJEN;
    chThdSleepMilliseconds(1);

    // Configure PH7 (Display Reset)
    palSetPadMode(GPIOH, 7, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

    // Configure PJ2 (DSI Tearing Effect) as AF13
    palSetPadMode(GPIOJ, 2, PAL_MODE_ALTERNATE(13) | PAL_STM32_OSPEED_HIGHEST);

    // Verify configuration
    dprintf("  GPIOJ->MODER[2]: 0x%lX (should be 2=AF)\n", (unsigned long)((GPIOJ->MODER >> (2 * 2)) & 0x3));
    dprintf("  GPIOJ->AFR[0][2]: 0x%lX (should be 13)\n", (unsigned long)((GPIOJ->AFRL >> (2 * 4)) & 0xF));

    // Reset the display
    palSetPad(GPIOH, 7);
    chThdSleepMilliseconds(10);
    dprintf("Resetting display...\n");
    palClearPad(GPIOH, 7);
    chThdSleepMilliseconds(20);
    palSetPad(GPIOH, 7);
    chThdSleepMilliseconds(120);

    dprintf("Step 1: Enabling DSI and LTDC clocks...\n");
    // Enable DSI and LTDC clocks
    RCC->APB2ENR |= RCC_APB2ENR_DSIEN;  // Enable DSI clock
    RCC->APB2ENR |= RCC_APB2ENR_LTDCEN; // Enable LTDC clock

    // Check RCC status
    dprintf("RCC->CR: 0x%08lX (HSE: %s)\n", (unsigned long)RCC->CR, (RCC->CR & RCC_CR_HSERDY) ? "ready" : "NOT READY");
    dprintf("RCC->APB2ENR: 0x%08lX\n", (unsigned long)RCC->APB2ENR);

    // Configure PLLSAIDIVR for 62.5MHz output (125MHz / 2)
    // This is used for LTDC pixel clock
    RCC->DCKCFGR &= ~RCC_DCKCFGR_PLLSAIDIVR_Msk;
    RCC->DCKCFGR |= RCC_DCKCFGR_PLLSAIDIVR_0; // Divide by 2

    dprintf("Step 2: Resetting DSI peripheral...\n");
    RCC->APB2RSTR |= RCC_APB2RSTR_DSIRST;
    chThdSleepMilliseconds(1);
    RCC->APB2RSTR &= ~RCC_APB2RSTR_DSIRST;
    chThdSleepMilliseconds(10);

    // Check initial DSI register state (may read zero before regulator enabled)
    dprintf("  Initial DSI register state:\n");
    dprintf("    WISR: 0x%08X\n", (unsigned int)DSI->WISR);
    dprintf("    CR: 0x%08X\n", (unsigned int)DSI->CR);
    dprintf("    WRPCR: 0x%08X\n", (unsigned int)DSI->WRPCR);

    dprintf("Step 3: Enabling DSI regulator...\n");
    // Enable DSI regulator FIRST (required before PLL can work)
    DSI->WRPCR |= DSI_WRPCR_REGEN;
    chThdSleepMilliseconds(10);

    // Wait for regulator to be ready
    timeout = 1000;
    while (!(DSI->WISR & DSI_WISR_RRS) && timeout > 0) {
        chThdSleepMilliseconds(1);
        timeout--;
    }

    if (timeout == 0) {
        dprintf("  ERROR: Regulator not ready\n");
        return false;
    }
    dprintf("  Regulator ready (WISR: 0x%08X)\n", (unsigned int)DSI->WISR);

    // Verify DSI registers are now accessible
    dprintf("  DSI register state after regulator enabled:\n");
    dprintf("    CR: 0x%08X\n", (unsigned int)DSI->CR);
    dprintf("    WRPCR: 0x%08X\n", (unsigned int)DSI->WRPCR);

    if (DSI->WRPCR == 0) {
        dprintf("  ERROR: WRPCR still reads zero - DSI not accessible!\n");
        return false;
    }

    dprintf("Step 4: Configuring DSI PLL...\n");
    // Configure DSI PLL
    // Input: 8MHz HSE
    // VCO = (8MHz / 2) × 2 × 125 = 1000MHz
    // Lane bit rate = 1000MHz / 2 / 1 = 500 Mbps
    // Byte clock = 62.5MHz

    // Disable PLL first
    DSI->WRPCR &= ~DSI_WRPCR_PLLEN;

    // Wait for PLL to be disabled
    timeout = 1000;
    while ((DSI->WISR & DSI_WISR_PLLLS) && timeout > 0) {
        chThdSleepMilliseconds(1);
        timeout--;
    }

    if (timeout == 0) {
        dprintf("ERROR: PLL disable timeout\n");
    }

    dprintf("WRPCR before config: 0x%08lX\n", (unsigned long)DSI->WRPCR);

    // Configure PLL
    // IDF = 2 (register value 1, since 0=/1, 1=/2, 2=/3, etc.)
    // NDIV = 125
    // ODF = 1 (register value 0, since 0=/1, 1=/2, 2=/4, 3=/8)
    uint32_t wrpcr_val = (125U << DSI_WRPCR_PLL_NDIV_Pos) | // NDIV = 125
                         (1U << DSI_WRPCR_PLL_IDF_Pos) |    // IDF = /2
                         (0U << DSI_WRPCR_PLL_ODF_Pos) |    // ODF = /1
                         DSI_WRPCR_REGEN |                  // Keep regulator enabled
                         DSI_WRPCR_PLLEN;                   // Enable PLL

    dprintf("Writing WRPCR: 0x%08lX\n", (unsigned long)wrpcr_val);
    dprintf("  NDIV=%lu, IDF=%lu, ODF=%lu\n", (wrpcr_val >> DSI_WRPCR_PLL_NDIV_Pos) & 0x1FF, (wrpcr_val >> DSI_WRPCR_PLL_IDF_Pos) & 0xF, (wrpcr_val >> DSI_WRPCR_PLL_ODF_Pos) & 0x3);

    DSI->WRPCR = wrpcr_val;

    dprintf("WRPCR after config: 0x%08lX\n", (unsigned long)DSI->WRPCR);

    dprintf("Step 5: Waiting for PLL lock...\n");
    // Wait for PLL to lock
    timeout = 1000;
    while (!(DSI->WISR & DSI_WISR_PLLLS) && timeout > 0) {
        chThdSleepMilliseconds(1);
        timeout--;
    }

    dprintf("WISR status: 0x%08lX\n", (unsigned long)DSI->WISR);
    dprintf("Timeout remaining: %lu\n", (unsigned long)timeout);

    if (timeout == 0) {
        // PLL failed to lock
        dprintf("ERROR: PLL failed to lock\n");
        return false;
    }
    dprintf("PLL locked successfully\n");

    dprintf("Step 6: Configuring DSI Host...\n");

    // Set number of lanes (2 lanes for OTM8009A)
    DSI->PCONFR &= ~DSI_PCONFR_NL;
    DSI->PCONFR |= 1U << DSI_PCONFR_NL_Pos;

    // Enable DSI wrapper and host
    DSI->WCR |= DSI_WCR_DSIEN;
    DSI->CR |= DSI_CR_EN;

    // CRITICAL: Configure TX Escape Clock Divider (required for command mode)
    // Lane byte clock = 62.5 MHz, target TX Escape clock = 15.62 MHz
    // Divider = 62500 / 15620 = 4
    DSI->CCR = 4U << DSI_CCR_TXECKDIV_Pos;
    dprintf("  TX Escape Clock Divider: 4 (DSI->CCR = 0x%08lX)\n", (unsigned long)DSI->CCR);

    // Configure timeout counters (disabled for testing)
    DSI->TCCR[0] = 0;
    DSI->TCCR[1] = 0;
    DSI->TCCR[2] = 0;
    DSI->TCCR[3] = 0;
    DSI->TCCR[4] = 0;
    DSI->TCCR[5] = 0;

    chThdSleepMilliseconds(10);

    // Verify configuration
    dprintf("  CR: 0x%08lX\n", (unsigned long)DSI->CR);
    dprintf("  WCR: 0x%08lX\n", (unsigned long)DSI->WCR);
    dprintf("  PCONFR: 0x%08lX\n", (unsigned long)DSI->PCONFR);

    // Configure PHY timings for 500 Mbps
    DSI->PCTLR |= DSI_PCTLR_DEN; // Digital enable
    DSI->PCTLR |= DSI_PCTLR_CKE; // Clock enable

    // Clock lane configuration
    DSI->CLCR &= ~(DSI_CLCR_DPCC | DSI_CLCR_ACR);
    DSI->CLCR |= DSI_CLCR_DPCC; // Enable D-PHY clock control

    // Calculate and set Unit Interval (UIX4) for PHY timing
    // UIX4 = (4000000 * IDF * (1 << ODF)) / ((HSE_kHz) * NDIV)
    // IDF=2, ODF=1, NDIV=125, HSE=8000 kHz
    // UIX4 = (4000000 * 2 * 1) / (8000 * 125) = 8
    DSI->WPCR[0] &= ~DSI_WPCR0_UIX4;
    DSI->WPCR[0] |= 8U;

    DSI->CLTCR = 0x00000707;
    DSI->DLTCR = 0x00000707;
    DSI->PCONFR |= DSI_PCONFR_SW_TIME;

    dprintf("  PCTLR: 0x%08lX\n", (unsigned long)DSI->PCTLR);

    // Configure flow control for command mode
    DSI->CMCR = DSI_CMCR_TEARE |  // Enable tearing effect acknowledge
                DSI_CMCR_ARE |    // Enable acknowledge request
                DSI_CMCR_GSW0TX | // Generic short write 0 in low-power
                DSI_CMCR_GSW1TX | // Generic short write 1 in low-power
                DSI_CMCR_GSW2TX | // Generic short write 2 in low-power
                DSI_CMCR_GSR0TX | // Generic short read 0 in low-power
                DSI_CMCR_GSR1TX | // Generic short read 1 in low-power
                DSI_CMCR_GSR2TX | // Generic short read 2 in low-power
                DSI_CMCR_GLWTX |  // Generic long write in low-power
                DSI_CMCR_DSW0TX | // DCS short write 0 in low-power
                DSI_CMCR_DSW1TX | // DCS short write 1 in low-power
                DSI_CMCR_DSR0TX | // DCS short read in low-power
                DSI_CMCR_DLWTX |  // DCS long write in low-power
                DSI_CMCR_MRDPS;   // Max read packet size in low-power

    // Enable low-power mode for commands
    DSI->LPMCR = DSI_LPMCR_VLPSIZE_Msk | // Max VACT size for low-power
                 DSI_LPMCR_LPSIZE_Msk;   // Max packet size for low-power

    dprintf("Step 7: Configuring virtual channel...\n");
    // Configure virtual channel and DPI color coding
    DSI->GVCIDR = 0; // Virtual channel ID 0

    // Enable regulators and wait
    chThdSleepMilliseconds(20);

    dprintf("Step 8: Sending exit sleep command...\n");

    // Wait for command FIFO to be empty
    timeout = 1000;
    while (!(DSI->GPSR & DSI_GPSR_CMDFE) && timeout > 0) {
        chThdSleepMicroseconds(100);
        timeout--;
    }
    dprintf("  GPSR before sleep out: 0x%08lX (CMDFE: %s)\n", (unsigned long)DSI->GPSR, (DSI->GPSR & DSI_GPSR_CMDFE) ? "empty" : "NOT empty");

    if (timeout == 0) {
        dprintf("  WARNING: Command FIFO not empty\n");
    }

    // Exit sleep mode
    dsi_dcs_write_0param(0x11);

    dprintf("  GPSR after sleep out: 0x%08lX\n", (unsigned long)DSI->GPSR);
    dprintf("  WISR: 0x%08lX\n", (unsigned long)DSI->WISR);

    // Sleep out requires 120ms
    chThdSleepMilliseconds(120);

    dprintf("Step 9: Reading panel IDs (before CMD2/Display ON)...\n");

    // Clear error flags before starting reads
    DSI->ISR[0] = 0xFFFFFFFF; // Clear all ISR0 errors
    DSI->ISR[1] = 0xFFFFFFFF; // Clear all ISR1 errors
    dprintf("  Cleared ISR error flags\n");

    // Clear any stale data in read FIFO
    int cleared = 0;
    while (!(DSI->GPSR & DSI_GPSR_PRDFE) && cleared < 10) {
        (void)DSI->GPDR;
        cleared++;
    }
    if (cleared > 0) dprintf("  Cleared %d stale FIFO entries before reads\n", cleared);

    // Note: HAL_DSI_Read only sets Max Return Packet Size for reads > 2 bytes
    // For 1-byte reads, it's not needed and may cause issues
    dprintf("  Skipping Set Max Return Packet Size (not needed for 1-byte reads per HAL)\n");

    // Read ID1 (0xDA) - should return 0x40 for OTM8009A
    dprintf("Reading ID1 (0xDA)...\n");
    DSI->GHCR = (0x06U) |     // DT: DCS Short Read
                (0U << 6) |   // VCID: 0
                (0xDAU << 8); // Command: Read ID1

    // Give panel time to process command
    chThdSleepMilliseconds(10);

    // Wait for payload read FIFO to have data (PRDFE=0)
    timeout = 10000;
    while ((DSI->GPSR & DSI_GPSR_PRDFE) && timeout > 0) {
        chThdSleepMicroseconds(10);
        timeout--;
    }

    dprintf("GPSR: 0x%08lX (timeout=%lu)\n", (unsigned long)DSI->GPSR, (unsigned long)timeout);
    dprintf("  GPSR bits: CMDFE=%d, CMDFF=%d, PWRFE=%d, PWRFF=%d, PRDFE=%d, PRDFF=%d, RCB=%d\n", !!(DSI->GPSR & DSI_GPSR_CMDFE), !!(DSI->GPSR & DSI_GPSR_CMDFF), !!(DSI->GPSR & DSI_GPSR_PWRFE), !!(DSI->GPSR & DSI_GPSR_PWRFF), !!(DSI->GPSR & DSI_GPSR_PRDFE), !!(DSI->GPSR & DSI_GPSR_PRDFF), !!(DSI->GPSR & DSI_GPSR_RCB));

    // Check for errors
    dprintf("  ISR0: 0x%08lX, ISR1: 0x%08lX\n", (unsigned long)DSI->ISR[0], (unsigned long)DSI->ISR[1]);

    if (timeout > 0 && !(DSI->GPSR & DSI_GPSR_PRDFE)) {
        // Read all FIFO entries to see if there are multiple packets
        uint32_t fifo_data[10];
        int      fifo_count = 0;
        while (!(DSI->GPSR & DSI_GPSR_PRDFE) && fifo_count < 10) {
            fifo_data[fifo_count] = DSI->GPDR;
            dprintf("  FIFO[%d]: 0x%08lX\n", fifo_count, (unsigned long)fifo_data[fifo_count]);
            fifo_count++;
        }
        dprintf("  Read %d FIFO entries\n", fifo_count);

        if (fifo_count == 0) {
            dprintf("  ERROR: FIFO empty despite PRDFE=0!\n");
            return false;
        }

        // Parse the first FIFO entry as the response packet
        uint32_t data0 = fifo_data[0];
        dprintf("GPDR[0]: 0x%08lX\n", (unsigned long)data0);

        // Parse the DSI response packet
        // Format: [DT][VC+WC][Data...][ECC]
        uint8_t dt = data0 & 0x3F;        // Data type in bits 5:0
        uint8_t wc = (data0 >> 8) & 0xFF; // Word count

        dprintf("  Packet DT: 0x%02X, WC: %u\n", dt, wc);

        // For a 1-byte read response, DT should be 0x11 or 0x12 (short read response)
        // Or 0x1C (long read response)
        if (dt == 0x11 || dt == 0x12) {
            // Short read response - data is in bits 15:8
            panel_id[0] = (data0 >> 8) & 0xFF;
            dprintf("  Short read response, ID1 = 0x%02X\n", panel_id[0]);
        } else if (dt == 0x1C || dt == 0x1A) {
            // Long read response - data follows
            panel_id[0] = (data0 >> 16) & 0xFF;
            dprintf("  Long read response, ID1 = 0x%02X\n", panel_id[0]);
        } else {
            dprintf("  Unexpected response type DT=0x%02X\n", dt);
            // Try to extract any non-zero byte
            for (int j = 0; j < 4; j++) {
                uint8_t byte = (data0 >> (j * 8)) & 0xFF;
                if (byte != 0 && byte != 0x1C && byte != 0x21 && byte != 0x11 && byte != 0x12) {
                    panel_id[0] = byte;
                    dprintf("  Found possible ID at byte %d: 0x%02X\n", j, byte);
                    break;
                }
            }
        }
    } else {
        dprintf("  No response or timeout\n");
    }

    dprintf("ID1: 0x%02X (expected 0x40)\n", panel_id[0]);

    chThdSleepMilliseconds(10);

    // Read ID2 (0xDB) - should return 0x00
    dprintf("Reading ID2 (0xDB)...\n");
    DSI->GHCR = (0x06U) | (0U << 6) | (0xDBU << 8);

    timeout = 10000;
    while ((DSI->GPSR & DSI_GPSR_PRDFE) && timeout > 0) {
        chThdSleepMicroseconds(10);
        timeout--;
    }

    dprintf("GPSR: 0x%08lX (timeout=%lu)\n", (unsigned long)DSI->GPSR, (unsigned long)timeout);

    if (timeout > 0 && !(DSI->GPSR & DSI_GPSR_PRDFE)) {
        uint32_t data0 = DSI->GPDR;
        dprintf("GPDR[0]: 0x%08lX\n", (unsigned long)data0);

        uint8_t dt = data0 & 0x3F;
        if (dt == 0x11 || dt == 0x12) {
            panel_id[1] = (data0 >> 8) & 0xFF;
        } else if (dt == 0x1C || dt == 0x1A) {
            panel_id[1] = (data0 >> 16) & 0xFF;
        } else {
            for (int j = 0; j < 4; j++) {
                uint8_t byte = (data0 >> (j * 8)) & 0xFF;
                if (byte != 0 && byte != 0x1C && byte != 0x21 && byte != 0x11 && byte != 0x12) {
                    panel_id[1] = byte;
                    break;
                }
            }
        }
    }

    dprintf("ID2: 0x%02X (expected 0x00)\n", panel_id[1]);

    // Clear FIFO
    cleared = 0;
    while (!(DSI->GPSR & DSI_GPSR_PRDFE) && cleared < 10) {
        (void)DSI->GPDR;
        cleared++;
    }
    if (cleared > 0) dprintf("Cleared %d FIFO entries\n", cleared);

    chThdSleepMilliseconds(10);

    // Read ID3 (0xDC) - should return 0x80
    dprintf("Reading ID3 (0xDC)...\n");
    DSI->GHCR = (0x06U) | (0U << 6) | (0xDCU << 8);

    timeout = 10000;
    while ((DSI->GPSR & DSI_GPSR_PRDFE) && timeout > 0) {
        chThdSleepMicroseconds(10);
        timeout--;
    }

    dprintf("GPSR: 0x%08lX (timeout=%lu)\n", (unsigned long)DSI->GPSR, (unsigned long)timeout);

    if (timeout > 0 && !(DSI->GPSR & DSI_GPSR_PRDFE)) {
        uint32_t data0 = DSI->GPDR;
        dprintf("GPDR[0]: 0x%08lX\n", (unsigned long)data0);

        uint8_t dt = data0 & 0x3F;
        if (dt == 0x11 || dt == 0x12) {
            panel_id[2] = (data0 >> 8) & 0xFF;
        } else if (dt == 0x1C || dt == 0x1A) {
            panel_id[2] = (data0 >> 16) & 0xFF;
        } else {
            for (int j = 0; j < 4; j++) {
                uint8_t byte = (data0 >> (j * 8)) & 0xFF;
                if (byte != 0 && byte != 0x1C && byte != 0x21 && byte != 0x11 && byte != 0x12) {
                    panel_id[2] = byte;
                    break;
                }
            }
        }
    }

    dprintf("ID3: 0x%02X (expected 0x80)\n", panel_id[2]);

    // Print final results
    dprintf("\n=== DSI Panel ID Test Results ===\n");
    dprintf("Panel IDs:\n");
    dprintf("  ID1 (0xDA): 0x%02X (expected 0x40)\n", panel_id[0]);
    dprintf("  ID2 (0xDB): 0x%02X (expected 0x00)\n", panel_id[1]);
    dprintf("  ID3 (0xDC): 0x%02X (expected 0x80)\n", panel_id[2]);

    // Check if IDs match OTM8009A
    if (panel_id[0] == 0x40 && panel_id[1] == 0x00 && panel_id[2] == 0x80) {
        dprintf("\n✓ SUCCESS: OTM8009A panel identified!\n");
        dprintf("DSI link is fully operational.\n");
        return true;
    } else if (panel_id[0] != 0 || panel_id[1] != 0 || panel_id[2] != 0) {
        dprintf("\n⚠ WARNING: Panel IDs don't match OTM8009A\n");
        dprintf("But DSI link is operational (received non-zero data)\n");
        return true;
    } else {
        dprintf("\n✗ FAILED: No valid panel IDs received\n");
        return false;
    }
}
