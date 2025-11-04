// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// Helper function to send DCS short write with 0 parameters
static void dsi_dcs_write_0param(uint8_t cmd) {
    DSI->GHCR = (0x05U) |           // DT: DCS short write, 0 param
                (0U << 6) |          // VCID: 0
                (cmd << 8);          // Command

    // Wait for command to be sent
    uint32_t timeout = 1000;
    while (!(DSI->GPSR & DSI_GPSR_CMDFE) && timeout > 0) {
        chThdSleepMicroseconds(100);
        timeout--;
    }
}

// Helper function to send DCS short write with 1 parameter
static void dsi_dcs_write_1param(uint8_t cmd, uint8_t param) {
    DSI->GHCR = (0x15U) |           // DT: DCS short write, 1 param
                (0U << 6) |          // VCID: 0
                (cmd << 8) |         // Command
                (param << 16);       // Parameter

    uint32_t timeout = 1000;
    while (!(DSI->GPSR & DSI_GPSR_CMDFE) && timeout > 0) {
        chThdSleepMicroseconds(100);
        timeout--;
    }
}

// Helper function to send Generic Long Write
static void dsi_generic_write(uint8_t cmd, const uint8_t *params, uint8_t len) {
    // For long write, we need to use GPDR to send the payload
    // Format: DT=0x29 (Generic Long Write), WC=len+1 (command + params)

    // First word contains command and up to 3 params
    uint32_t data = cmd;
    for (int i = 0; i < len && i < 3; i++) {
        data |= (uint32_t)params[i] << (8 * (i + 1));
    }
    DSI->GPDR = data;

    // Write remaining params if any
    for (int i = 3; i < len; i += 4) {
        data = 0;
        for (int j = 0; j < 4 && (i + j) < len; j++) {
            data |= (uint32_t)params[i + j] << (8 * j);
        }
        DSI->GPDR = data;
    }

    // Send the long write command with word count
    DSI->GHCR = (0x29U) |               // DT: Generic Long Write
                (0U << 6) |              // VCID: 0
                ((len + 1) << 8);        // WC: length including command byte

    uint32_t timeout = 1000;
    while (!(DSI->GPSR & DSI_GPSR_CMDFE) && timeout > 0) {
        chThdSleepMicroseconds(100);
        timeout--;
    }
}

// Minimal OTM8009A initialization to enable CMD2 and allow ID reads
static bool otm8009a_enable_cmd2(void) {
    dprintf("Enabling OTM8009A CMD2 mode...\n");

    // Check GPSR before we start
    dprintf("  Initial GPSR: 0x%08lX\n", (unsigned long)DSI->GPSR);

    // Step 1: Send NOP to shift address to 0x00
    dprintf("  Sending NOP (0x00)...\n");
    dsi_dcs_write_0param(0x00);  // NOP
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
    dsi_dcs_write_1param(0x00, 0x80);  // NOP with parameter 0x80
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
    uint8_t panel_id[3] = {0, 0, 0};
    uint32_t timeout;

    dprintf("Step 0: Configuring display GPIO pins...\n");

    // Configure PH7 as output (Display Reset)
    palSetPadMode(GPIOH, 7, PAL_MODE_OUTPUT_PUSHPULL);

    // Configure PJ2 as alternate function for DSI TE (AF13)
    palSetPadMode(GPIOJ, 2, PAL_MODE_ALTERNATE(13));

    // Ensure reset pin is high first
    palSetPad(GPIOH, 7);
    chThdSleepMilliseconds(10);

    // Reset the display (active low)
    dprintf("Resetting display...\n");
    palClearPad(GPIOH, 7);                // Set PH7 low (reset active)
    chThdSleepMilliseconds(20);
    palSetPad(GPIOH, 7);                  // Set PH7 high (reset inactive)
    chThdSleepMilliseconds(120);          // Longer delay after reset

    dprintf("Step 1: Enabling DSI and LTDC clocks...\n");
    // Enable DSI and LTDC clocks
    RCC->APB2ENR |= RCC_APB2ENR_DSIEN;   // Enable DSI clock
    RCC->APB2ENR |= RCC_APB2ENR_LTDCEN;  // Enable LTDC clock

    // Check RCC status
    dprintf("RCC->CR: 0x%08lX (HSE: %s)\n",
            (unsigned long)RCC->CR,
            (RCC->CR & RCC_CR_HSERDY) ? "ready" : "NOT READY");
    dprintf("RCC->APB2ENR: 0x%08lX\n", (unsigned long)RCC->APB2ENR);

    // Configure PLLSAIDIVR for 62.5MHz output (125MHz / 2)
    // This is used for LTDC pixel clock
    RCC->DCKCFGR &= ~RCC_DCKCFGR_PLLSAIDIVR_Msk;
    RCC->DCKCFGR |= RCC_DCKCFGR_PLLSAIDIVR_0;  // Divide by 2

    dprintf("Step 2: Resetting DSI peripheral...\n");
    // Reset DSI peripheral
    RCC->APB2RSTR |= RCC_APB2RSTR_DSIRST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_DSIRST;

    // Wait for reset to complete
    chThdSleepMilliseconds(10);

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
        dprintf("ERROR: Regulator not ready\n");
        return false;
    }
    dprintf("Regulator ready\n");

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
    uint32_t wrpcr_val = (125U << DSI_WRPCR_PLL_NDIV_Pos) |   // NDIV = 125
                         (1U << DSI_WRPCR_PLL_IDF_Pos) |      // IDF = /2
                         (0U << DSI_WRPCR_PLL_ODF_Pos) |      // ODF = /1
                         DSI_WRPCR_REGEN |                     // Keep regulator enabled
                         DSI_WRPCR_PLLEN;                      // Enable PLL

    dprintf("Writing WRPCR: 0x%08lX\n", (unsigned long)wrpcr_val);
    dprintf("  NDIV=%lu, IDF=%lu, ODF=%lu\n",
            (wrpcr_val >> DSI_WRPCR_PLL_NDIV_Pos) & 0x1FF,
            (wrpcr_val >> DSI_WRPCR_PLL_IDF_Pos) & 0xF,
            (wrpcr_val >> DSI_WRPCR_PLL_ODF_Pos) & 0x3);

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
    // Configure DSI Host
    // Set number of lanes (2 lanes for OTM8009A)
    DSI->PCONFR &= ~DSI_PCONFR_NL;
    DSI->PCONFR |= 1U << DSI_PCONFR_NL_Pos;  // 2 lanes (0=1 lane, 1=2 lanes)

    // Enable DSI wrapper
    DSI->WCR |= DSI_WCR_DSIEN;

    // Enable DSI host
    DSI->CR |= DSI_CR_EN;

    // Wait for DSI to be ready
    chThdSleepMilliseconds(10);

    // Configure PHY timings for 500 Mbps
    DSI->PCTLR |= DSI_PCTLR_DEN;   // Enable digital section
    DSI->PCTLR |= DSI_PCTLR_CKE;   // Enable clock lane
    DSI->CLCR &= ~DSI_CLCR_ACR;    // Disable automatic clock lane control

    // Configure timing
    DSI->CLTCR = 0x00000707;  // Clock lane timings
    DSI->DLTCR = 0x00000707;  // Data lane timings
    DSI->PCONFR |= DSI_PCONFR_SW_TIME;  // Use software timing

    // Configure flow control for command mode
    DSI->CMCR = DSI_CMCR_TEARE |       // Enable tearing effect acknowledge
                DSI_CMCR_ARE   |       // Enable acknowledge request
                DSI_CMCR_GSW0TX |      // Generic short write 0 in low-power
                DSI_CMCR_GSW1TX |      // Generic short write 1 in low-power
                DSI_CMCR_GSW2TX |      // Generic short write 2 in low-power
                DSI_CMCR_GSR0TX |      // Generic short read 0 in low-power
                DSI_CMCR_GSR1TX |      // Generic short read 1 in low-power
                DSI_CMCR_GSR2TX |      // Generic short read 2 in low-power
                DSI_CMCR_GLWTX  |      // Generic long write in low-power
                DSI_CMCR_DSW0TX |      // DCS short write 0 in low-power
                DSI_CMCR_DSW1TX |      // DCS short write 1 in low-power
                DSI_CMCR_DSR0TX |      // DCS short read in low-power
                DSI_CMCR_DLWTX  |      // DCS long write in low-power
                DSI_CMCR_MRDPS;        // Max read packet size in low-power

    // Enable low-power mode for commands
    DSI->LPMCR = DSI_LPMCR_VLPSIZE_Msk |  // Max VACT size for low-power
                 DSI_LPMCR_LPSIZE_Msk;     // Max packet size for low-power

    dprintf("Step 7: Configuring virtual channel...\n");
    // Configure virtual channel and DPI color coding
    DSI->GVCIDR = 0;  // Virtual channel ID 0

    // Enable regulators and wait
    chThdSleepMilliseconds(20);

    dprintf("Step 8: Sending exit sleep command...\n");

    // Wait for command FIFO to be empty
    timeout = 1000;
    while (!(DSI->GPSR & DSI_GPSR_CMDFE) && timeout > 0) {
        chThdSleepMicroseconds(100);
        timeout--;
    }
    dprintf("Command FIFO status before sleep out: GPSR=0x%08lX\n", (unsigned long)DSI->GPSR);

    // Exit sleep mode - DCS Short Write, no parameters: 0x11
    dsi_dcs_write_0param(0x11);  // Sleep out

    dprintf("Sleep out sent, GPSR=0x%08lX\n", (unsigned long)DSI->GPSR);

    // Sleep out requires 120ms
    chThdSleepMilliseconds(120);

    // Turn display ON (0x29)
    dprintf("Sending display ON command...\n");
    dsi_dcs_write_0param(0x29);  // Display ON

    chThdSleepMilliseconds(20);

    // Enable CMD2 mode to allow reads
    if (!otm8009a_enable_cmd2()) {
        dprintf("Failed to enable CMD2 mode\n");
        return false;
    }

    dprintf("Step 9: Reading panel IDs...\n");

    // Set maximum return packet size to 1 byte
    DSI->GHCR = (0x37U) |                         // DT: Set Max Return Packet Size
                (0U << 6) |                        // VCID: 0
                (1U << 8);                         // Word count: 1 byte

    timeout = 1000;
    while (!(DSI->GPSR & DSI_GPSR_CMDFE) && timeout > 0) {
        chThdSleepMicroseconds(10);
        timeout--;
    }
    chThdSleepMilliseconds(5);

    // Read ID1 (0xDA) - should return 0x40 for OTM8009A
    dprintf("Reading ID1 (0xDA)...\n");
    DSI->GHCR = (0x06U) |                         // DT: DCS read
                (0U << 6) |                        // VCID: 0
                (0xDAU << 8);                      // Command: Read ID1

    timeout = 10000;
    while (!(DSI->GPSR & (1U << 6)) && timeout > 0) {  // Wait for RCB
        chThdSleepMicroseconds(10);
        timeout--;
    }

    dprintf("GPSR: 0x%08lX (timeout=%lu)\n", (unsigned long)DSI->GPSR, (unsigned long)timeout);

    if (timeout > 0 && (DSI->GPSR & (1U << 6))) {  // RCB set and didn't timeout
        // Read response - but safely with a single read first
        uint32_t data0 = DSI->GPDR;
        dprintf("GPDR[0]: 0x%08lX\n", (unsigned long)data0);

        // Parse the DSI response packet
        // Format: [DT][VC+WC][Data...][ECC]
        uint8_t dt = data0 & 0x3F;  // Data type in bits 5:0
        uint8_t wc = (data0 >> 8) & 0xFF;  // Word count

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

    // Clear FIFO safely - limit reads
    int cleared = 0;
    for (int i = 0; i < 5; i++) {
        if (!(DSI->GPSR & (1U << 6))) break;
        (void)DSI->GPDR;
        cleared++;
    }
    if (cleared > 0) dprintf("Cleared %d FIFO entries\n", cleared);

    chThdSleepMilliseconds(10);

    // Read ID2 (0xDB) - should return 0x00
    dprintf("Reading ID2 (0xDB)...\n");
    DSI->GHCR = (0x06U) |
                (0U << 6) |
                (0xDBU << 8);

    timeout = 10000;
    while (!(DSI->GPSR & (1U << 6)) && timeout > 0) {
        chThdSleepMicroseconds(10);
        timeout--;
    }

    dprintf("GPSR: 0x%08lX (timeout=%lu)\n", (unsigned long)DSI->GPSR, (unsigned long)timeout);

    if (timeout > 0 && (DSI->GPSR & (1U << 6))) {
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
    for (int i = 0; i < 5; i++) {
        if (!(DSI->GPSR & (1U << 6))) break;
        (void)DSI->GPDR;
        cleared++;
    }
    if (cleared > 0) dprintf("Cleared %d FIFO entries\n", cleared);

    chThdSleepMilliseconds(10);

    // Read ID3 (0xDC) - should return 0x80
    dprintf("Reading ID3 (0xDC)...\n");
    DSI->GHCR = (0x06U) |
                (0U << 6) |
                (0xDCU << 8);

    timeout = 10000;
    while (!(DSI->GPSR & (1U << 6)) && timeout > 0) {
        chThdSleepMicroseconds(10);
        timeout--;
    }

    dprintf("GPSR: 0x%08lX (timeout=%lu)\n", (unsigned long)DSI->GPSR, (unsigned long)timeout);

    if (timeout > 0 && (DSI->GPSR & (1U << 6))) {
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
