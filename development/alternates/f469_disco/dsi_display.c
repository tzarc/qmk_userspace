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

// Helper function to read 1 byte via DCS read
static uint8_t dsi_dcs_read_1byte(uint8_t cmd) {
    uint32_t timeout;

    // Clear any error flags from previous operations
    // These are write-1-to-clear registers
    DSI->ISR[0] = 0xFFFFFFFF;  // Clear all ISR0 errors
    DSI->ISR[1] = 0xFFFFFFFF;  // Clear all ISR1 errors

    // Completely drain the read FIFO before starting
    timeout = 100;
    while (!(DSI->GPSR & DSI_GPSR_PRDFE) && timeout > 0) {
        (void)DSI->GPDR;  // Dummy read to clear FIFO
        timeout--;
    }

    // Send DCS read command (0x06 = DCS short read, 0 parameters)
    // Format: DataType | (VirtualChannel << 6) | (Data0 << 8) | (Data1 << 16)
    DSI->GHCR = (0x06U) | (0U << 6) | (cmd << 8) | (0U << 16);

    // Small delay to allow command to be transmitted
    chThdSleepMicroseconds(100);

    // Wait for response data to arrive in FIFO
    // Wait while FIFO is empty (PRDFE=1) AND while read command is busy (RCB=1)
    timeout = 10000;
    while (((DSI->GPSR & DSI_GPSR_PRDFE) || (DSI->GPSR & DSI_GPSR_RCB)) && timeout > 0) {
        chThdSleepMicroseconds(10);
        timeout--;
    }

    if (timeout == 0) {
        dprintf("      Read timeout for cmd 0x%02X\n", cmd);
        dprintf("      GPSR: 0x%08lX (PRDFE=%lu, RCB=%lu)\n",
                (unsigned long)DSI->GPSR,
                (unsigned long)((DSI->GPSR & DSI_GPSR_PRDFE) ? 1 : 0),
                (unsigned long)((DSI->GPSR & DSI_GPSR_RCB) ? 1 : 0));
        dprintf("      ISR[0]: 0x%08lX\n", (unsigned long)DSI->ISR[0]);
        dprintf("      ISR[1]: 0x%08lX\n", (unsigned long)DSI->ISR[1]);
        return 0xFF;  // Timeout - return error value
    }

    // Read all available data from FIFO (there might be multiple words)
    uint32_t response = 0;
    int word_count = 0;
    while (!(DSI->GPSR & DSI_GPSR_PRDFE) && word_count < 4) {
        response = DSI->GPDR;
        word_count++;
        dprintf("      GPDR word %d: 0x%08lX\n", word_count, (unsigned long)response);
    }

    if (word_count == 0) {
        dprintf("      No data in FIFO!\n");
        return 0xFF;
    }

    // Extract data byte from the response
    // The response format for a DCS short read is:
    // Byte 0: Data ID (response packet type)
    // Byte 1: Actual data byte
    // Byte 2: ECC
    uint8_t byte0 = (response >> 0) & 0xFF;
    uint8_t byte1 = (response >> 8) & 0xFF;
    uint8_t byte2 = (response >> 16) & 0xFF;
    uint8_t byte3 = (response >> 24) & 0xFF;

    dprintf("      Response bytes: [0]=0x%02X, [1]=0x%02X, [2]=0x%02X, [3]=0x%02X\n",
            byte0, byte1, byte2, byte3);

    // Check for errors after read
    if (DSI->ISR[0] != 0 || DSI->ISR[1] != 0) {
        dprintf("      Errors after read: ISR[0]=0x%08lX, ISR[1]=0x%08lX\n",
                (unsigned long)DSI->ISR[0], (unsigned long)DSI->ISR[1]);
    }

    // DSI Short Read Response format:
    // Byte 0: Data ID (packet type: 0x11/0x12=generic, 0x21/0x22=DCS)
    // Byte 1: Actual data (for 1-byte response)
    // Byte 2+: ECC or additional data
    //
    // Check packet type and extract data accordingly
    if (byte0 == 0x21 || byte0 == 0x11) {
        // Short read response with 1 byte - data is in byte1
        dprintf("      Short read response (DT=0x%02X), data=0x%02X\n", byte0, byte1);
        return byte1;
    } else if (byte0 == 0x22 || byte0 == 0x12) {
        // Short read response with 2 bytes - first data byte is in byte1
        dprintf("      Short read response 2-byte (DT=0x%02X), data=0x%02X\n", byte0, byte1);
        return byte1;
    } else if (byte0 == 0x02) {
        // Acknowledge and Error Report - no data
        dprintf("      Acknowledge/Error (DT=0x%02X)\n", byte0);
        return 0xFF;
    } else {
        // Unknown packet type - panel may return raw data in byte0
        // NT35510/OTM8009A sometimes doesn't use standard DSI packet format
        dprintf("      Unknown response format, bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n", byte0, byte1, byte2, byte3);
        // Return first non-zero byte as the data
        if (byte0 != 0) {
            dprintf("      Returning byte0=0x%02X as data\n", byte0);
            return byte0;
        } else if (byte1 != 0) {
            dprintf("      Returning byte1=0x%02X as data\n", byte1);
            return byte1;
        }
        dprintf("      All bytes zero, returning 0x00\n");
        return byte0;
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

__attribute__((used)) bool dsi_test_read_panel_id(void) {
    //uint8_t  panel_id[3] = {0, 0, 0};
    uint32_t timeout;

    // CRITICAL: First output to verify function is called
    dprintf("\n\n=== DSI Display Initialization Starting ===\n");
    dprintf("Function entry confirmed\n");

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

    // Configure PLLSAIDIVR for 27.429MHz LTDC pixel clock per ST F469-Discovery
    // PLLSAIR output = 384MHz / 7 = 54.857MHz, then /2 = 27.429MHz
    RCC->DCKCFGR &= ~RCC_DCKCFGR_PLLSAIDIVR_Msk;
    RCC->DCKCFGR |= RCC_DCKCFGR_PLLSAIDIVR_0; // Divide by 2 (bits [17:16] = 01)

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
    // Configure DSI PLL per ST F469-Discovery reference (500MHz PHY)
    // Input: 8MHz HSE
    // IDF = 2: VCO in = 8MHz / 2 = 4MHz
    // NDIV = 125: VCO = 4MHz × 125 = 500MHz
    // ODF = 1: PHY = 500MHz / 1 = 500MHz
    // Lane byte clock = 500MHz / 8 = 62.5MHz

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

    // Note: DSI wrapper and host will be enabled AFTER LTDC initialization

    // CRITICAL: Configure TX Escape Clock Divider (required for command mode)
    // Lane byte clock = 62.5 MHz, target TX Escape clock ~15.625 MHz
    // Divider = 62.5MHz / 15.625MHz = 4
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
    // IDF=2, ODF=0 (÷1), NDIV=125, HSE=8000 kHz
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

    // Configure Protocol Configuration Register (PCR) for flow control
    // This is CRITICAL for read operations - enables BTA (Bus Turn Around)
    DSI->PCR = DSI_PCR_BTAE |    // Bus Turn Around Enable (required for reads!)
               DSI_PCR_ECCRXE |  // ECC reception enable
               DSI_PCR_CRCRXE |  // CRC reception enable
               DSI_PCR_ETRXE |   // EoTp reception enable
               DSI_PCR_ETTXE;    // EoTp transmission enable

    dprintf("  PCR configured for flow control (BTA enabled): 0x%08lX\n", (unsigned long)DSI->PCR);

    dprintf("Step 7: Configuring DSI Video Mode...\n");
    // Configure virtual channel and DPI color coding
    DSI->GVCIDR = 0; // Virtual channel ID 0

    // Configure DSI Video Mode for OTM8009A (800x480 landscape)
    // Timing parameters from ST reference
    uint32_t VSA = 1;   // OTM8009A_480X800_VSYNC
    uint32_t VBP = 15;  // OTM8009A_480X800_VBP
    uint32_t VFP = 16;  // OTM8009A_480X800_VFP
    uint32_t HSA = 2;   // OTM8009A_480X800_HSYNC
    uint32_t HBP = 34;  // OTM8009A_480X800_HBP
    uint32_t HFP = 34;  // OTM8009A_480X800_HFP
    uint32_t HACT = 800;
    uint32_t VACT = 480;

    // Lane byte clock = 62.5 MHz, LCD clock = 27.429 MHz
    uint32_t laneByteClk_kHz = 62500;
    uint32_t LcdClock = 27429;

    // Configure video mode packet size
    DSI->VPCR &= ~DSI_VPCR_VPSIZE;
    DSI->VPCR = HACT; // Packet size = horizontal active width

    // Configure video mode timings (converted to lane byte clock cycles)
    DSI->VHSACR = (HSA * laneByteClk_kHz) / LcdClock;
    DSI->VHBPCR = (HBP * laneByteClk_kHz) / LcdClock;
    DSI->VLCR = ((HACT + HSA + HBP + HFP) * laneByteClk_kHz) / LcdClock;

    DSI->VVSACR = VSA;
    DSI->VVBPCR = VBP;
    DSI->VVFPCR = VFP;
    DSI->VVACR = VACT;

    // Configure video mode type (burst mode)
    // VMT bits: 00=non-burst sync pulses, 01=non-burst sync events, 10=burst
    DSI->VMCR &= ~DSI_VMCR_VMT;
    DSI->VMCR |= (2U << 0); // Burst mode (VMT = 10b)

    // Enable LP command transmission in video mode during blanking periods
    DSI->VMCR |= DSI_VMCR_LPCE;        // Enable LP commands in video mode
    DSI->VMCR |= DSI_VMCR_LPHFPE;      // LP during horizontal front porch
    DSI->VMCR |= DSI_VMCR_LPHBPE;      // LP during horizontal back porch
    DSI->VMCR |= DSI_VMCR_LPVAE;       // LP during vertical active
    DSI->VMCR |= DSI_VMCR_LPVFPE;      // LP during vertical front porch
    DSI->VMCR |= DSI_VMCR_LPVBPE;      // LP during vertical back porch
    DSI->VMCR |= DSI_VMCR_LPVSAE;      // LP during vertical sync (corrected)

    // Set largest packet size for LP commands (using LPMCR register)
    DSI->LPMCR &= ~DSI_LPMCR_LPSIZE;
    DSI->LPMCR |= 16U << DSI_LPMCR_LPSIZE_Pos;  // 16 bytes max

    dprintf("  Video mode: %lux%lu, HSA=%lu, HBP=%lu, HFP=%lu, VSA=%lu, VBP=%lu, VFP=%lu\n",
            (unsigned long)HACT, (unsigned long)VACT,
            (unsigned long)HSA, (unsigned long)HBP, (unsigned long)HFP,
            (unsigned long)VSA, (unsigned long)VBP, (unsigned long)VFP);

    dprintf("Step 8: Initializing LTDC...\n");

    // Enable LTDC clock (already enabled in Step 1)
    // Configure LTDC timing (derived from DSI video mode)
    LTDC->SSCR = ((HSA - 1) << LTDC_SSCR_HSW_Pos) | ((VSA - 1) << LTDC_SSCR_VSH_Pos);
    LTDC->BPCR = ((HSA + HBP - 1) << LTDC_BPCR_AHBP_Pos) | ((VSA + VBP - 1) << LTDC_BPCR_AVBP_Pos);
    LTDC->AWCR = ((HACT + HSA + HBP - 1) << LTDC_AWCR_AAW_Pos) | ((VACT + VSA + VBP - 1) << LTDC_AWCR_AAH_Pos);
    LTDC->TWCR = ((HACT + HSA + HBP + HFP - 1) << LTDC_TWCR_TOTALW_Pos) | ((VACT + VSA + VBP + VFP - 1) << LTDC_TWCR_TOTALH_Pos);

    // Configure synchronization polarity
    LTDC->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);
    LTDC->GCR |= LTDC_GCR_PCPOL; // Pixel clock polarity

    // Set background color (black)
    LTDC->BCCR = 0x00000000;

    // Enable LTDC
    LTDC->GCR |= LTDC_GCR_LTDCEN;

    dprintf("  LTDC initialized: SSCR=0x%08lX, BPCR=0x%08lX, AWCR=0x%08lX, TWCR=0x%08lX\n",
            (unsigned long)LTDC->SSCR, (unsigned long)LTDC->BPCR,
            (unsigned long)LTDC->AWCR, (unsigned long)LTDC->TWCR);

    dprintf("Step 9: Starting DSI in video mode...\n");

    // Start DSI wrapper (critical: must be done AFTER LTDC init)
    DSI->WCR |= DSI_WCR_DSIEN;
    DSI->CR |= DSI_CR_EN;

    dprintf("  DSI started in video mode\n");

    // Wait for DSI to stabilize
    chThdSleepMilliseconds(20);

    dprintf("Step 10: Initializing NT35510 panel...\n");

    // NT35510 initialization sequence from official ST BSP driver
    // Reference: lv_port_stm32f469_disco/Drivers/BSP/Components/nt35510/nt35510.c

    // ========================================
    // Proprietary Initialization (Page 1)
    // ========================================
    dprintf("  Enabling MCS Page 1...\n");
    const uint8_t page1_enable[] = {0x55, 0xAA, 0x52, 0x08, 0x01};
    dsi_generic_write(0xF0, page1_enable, 5);

    // Power settings
    dprintf("  Configuring power settings...\n");
    const uint8_t avdd[] = {0x03, 0x03, 0x03};        // AVDD: 5.2V
    dsi_generic_write(0xB0, avdd, 3);
    const uint8_t avdd_ratio[] = {0x46, 0x46, 0x46};  // AVDD ratio
    dsi_generic_write(0xB6, avdd_ratio, 3);
    const uint8_t avee[] = {0x03, 0x03, 0x03};        // AVEE: -5.2V
    dsi_generic_write(0xB1, avee, 3);
    const uint8_t avee_ratio[] = {0x36, 0x36, 0x36};  // AVEE ratio
    dsi_generic_write(0xB7, avee_ratio, 3);
    const uint8_t vcl[] = {0x00, 0x00, 0x02};         // VCL: -2.5V
    dsi_generic_write(0xB2, vcl, 3);
    const uint8_t vcl_ratio[] = {0x26, 0x26, 0x26};   // VCL ratio
    dsi_generic_write(0xB8, vcl_ratio, 3);
    dsi_dcs_write_1param(0xBF, 0x01);                 // VGH: 15V (Free pump)
    const uint8_t vgh[] = {0x09, 0x09, 0x09};         // VGH
    dsi_generic_write(0xB3, vgh, 3);
    const uint8_t vgh_ratio[] = {0x36, 0x36, 0x36};   // VGH ratio
    dsi_generic_write(0xB9, vgh_ratio, 3);
    const uint8_t vgl[] = {0x08, 0x08, 0x08};         // VGL_REG: -10V
    dsi_generic_write(0xB5, vgl, 3);
    const uint8_t vgl_ratio[] = {0x26, 0x26, 0x26};   // VGLX ratio
    dsi_generic_write(0xBA, vgl_ratio, 3);
    const uint8_t vgmp[] = {0x00, 0x80, 0x00};        // VGMP/VGSP: 4.5V/0V
    dsi_generic_write(0xBC, vgmp, 3);
    const uint8_t vgmn[] = {0x00, 0x80, 0x00};        // VGMN/VGSN: -4.5V/0V
    dsi_generic_write(0xBD, vgmn, 3);
    const uint8_t vcom[] = {0x00, 0x50};              // VCOM: -1.325V
    dsi_generic_write(0xBE, vcom, 2);

    // ========================================
    // Proprietary DCS Initialization (Page 0)
    // ========================================
    dprintf("  Enabling MCS Page 0...\n");
    const uint8_t page0_enable[] = {0x55, 0xAA, 0x52, 0x08, 0x00};
    dsi_generic_write(0xF0, page0_enable, 5);

    const uint8_t dispctrl[] = {0xFC, 0x00};          // Display control
    dsi_generic_write(0xB1, dispctrl, 2);
    dsi_dcs_write_1param(0xB6, 0x03);                 // Src hold time
    dsi_dcs_write_1param(0xB5, 0x51);
    const uint8_t gate_eq[] = {0x00, 0x00};           // Gate EQ control
    dsi_generic_write(0xB7, gate_eq, 2);
    const uint8_t src_eq[] = {0x01, 0x02, 0x02, 0x02}; // Src EQ control (Mode2)
    dsi_generic_write(0xB8, src_eq, 4);
    const uint8_t inv_mode[] = {0x00, 0x00, 0x00};    // Inv. mode (2-dot)
    dsi_generic_write(0xBC, inv_mode, 3);
    const uint8_t disp_timing[] = {0x03, 0x00, 0x00}; // Display timing
    dsi_generic_write(0xCC, disp_timing, 3);
    dsi_dcs_write_1param(0xBA, 0x01);

    // Tear on
    dsi_dcs_write_0param(0x35);

    // Set pixel format to RGB888
    dprintf("  Setting pixel format to RGB888...\n");
    dsi_dcs_write_1param(0x3A, 0x77);  // COLMOD - RGB888

    // Add delay for MADCTL to take effect
    chThdSleepMilliseconds(200);

    // ========================================
    // Standard DCS Initialization
    // ========================================
    // Configure landscape orientation (800x480)
    dprintf("  Setting landscape orientation...\n");
    dsi_dcs_write_1param(0x36, 0x60);  // MADCTL - Landscape

    // Set column address for landscape (0-799)
    const uint8_t caset[] = {0x00, 0x00, 0x03, 0x1F};
    dsi_generic_write(0x2A, caset, 4);

    // Set page address for landscape (0-479)
    const uint8_t paset[] = {0x00, 0x00, 0x01, 0xDF};
    dsi_generic_write(0x2B, paset, 4);

    // Exit sleep mode
    dprintf("  Sending Sleep Out...\n");
    dsi_dcs_write_0param(0x11);  // Sleep Out
    chThdSleepMilliseconds(120);

    // Set pixel format again after sleep out
    dsi_dcs_write_1param(0x3A, 0x77);  // COLMOD - RGB888

    // ========================================
    // CABC (Content Adaptive Backlight Control)
    // ========================================
    dsi_dcs_write_1param(0x51, 0x7F);  // Brightness: intermediate
    dsi_dcs_write_1param(0x53, 0x2C);  // Brightness Control Block, Display Dimming & BackLight on
    dsi_dcs_write_1param(0x55, 0x02);  // Image Content based Adaptive Brightness [Still Picture]
    dsi_dcs_write_1param(0x5E, 0xFF);  // CABC minimum brightness

    // Display ON
    dprintf("  Enabling display...\n");
    dsi_dcs_write_0param(0x29);  // Display On

    // Start GRAM write
    dprintf("  Starting GRAM write...\n");
    dsi_dcs_write_0param(0x2C);  // Write Memory Start

    dprintf("NT35510 initialization complete!\n");

    // ========================================
    // Step 10b: Configure LTDC Layer 1 with red framebuffer
    // ========================================
    dprintf("\nStep 10b: Configuring LTDC Layer 1 with red framebuffer...\n");

    // Framebuffer in SDRAM (F469-Discovery SDRAM starts at 0xC0000000)
    // Using RGB888 format (3 bytes per pixel)
    // 800 x 480 x 3 = 1,152,000 bytes
    uint8_t *framebuffer = (uint8_t *)0xC0000000;

    // Fill framebuffer with RED (RGB888: R=0xFF, G=0x00, B=0x00)
    dprintf("  Filling framebuffer with RED...\n");
    for (uint32_t i = 0; i < 800 * 480; i++) {
        framebuffer[i * 3 + 0] = 0x00;  // Blue
        framebuffer[i * 3 + 1] = 0x00;  // Green
        framebuffer[i * 3 + 2] = 0xFF;  // Red
    }
    dprintf("  Framebuffer filled at 0x%08lX\n", (unsigned long)framebuffer);

    // Configure LTDC Layer 1
    // Window horizontal position
    uint32_t hstart = HSA + HBP;
    uint32_t hstop = HSA + HBP + HACT - 1;
    LTDC_Layer1->WHPCR = (hstop << 16) | hstart;

    // Window vertical position
    uint32_t vstart = VSA + VBP;
    uint32_t vstop = VSA + VBP + VACT - 1;
    LTDC_Layer1->WVPCR = (vstop << 16) | vstart;

    // Pixel format: RGB888
    LTDC_Layer1->PFCR = 1;  // 1 = RGB888

    // Constant alpha: fully opaque
    LTDC_Layer1->CACR = 0xFF;

    // Default color (black, fully transparent)
    LTDC_Layer1->DCCR = 0x00000000;

    // Blending factors: use constant alpha
    LTDC_Layer1->BFCR = (4 << 8) | 5;  // BF1=constant alpha, BF2=1-constant alpha

    // Color frame buffer start address
    LTDC_Layer1->CFBAR = (uint32_t)framebuffer;

    // Color frame buffer line length and pitch
    // Line length = active width * bytes per pixel + 3 (must be >= pitch)
    // Pitch = active width * bytes per pixel
    uint32_t pitch = HACT * 3;  // 800 * 3 = 2400 bytes
    LTDC_Layer1->CFBLR = (pitch << 16) | (pitch + 3);

    // Color frame buffer line number
    LTDC_Layer1->CFBLNR = VACT;  // 480 lines

    // Enable Layer 1
    LTDC_Layer1->CR |= LTDC_LxCR_LEN;

    // Reload LTDC configuration immediately
    LTDC->SRCR = LTDC_SRCR_IMR;

    dprintf("  LTDC Layer 1 configured:\n");
    dprintf("    WHPCR: 0x%08lX (H: %lu-%lu)\n", (unsigned long)LTDC_Layer1->WHPCR, (unsigned long)hstart, (unsigned long)hstop);
    dprintf("    WVPCR: 0x%08lX (V: %lu-%lu)\n", (unsigned long)LTDC_Layer1->WVPCR, (unsigned long)vstart, (unsigned long)vstop);
    dprintf("    CFBAR: 0x%08lX\n", (unsigned long)LTDC_Layer1->CFBAR);
    dprintf("    CFBLR: 0x%08lX (pitch=%lu)\n", (unsigned long)LTDC_Layer1->CFBLR, (unsigned long)pitch);
    dprintf("    CR: 0x%08lX\n", (unsigned long)LTDC_Layer1->CR);

    // Step 11: Read panel IDs to verify communication
    dprintf("\nStep 11: Reading panel IDs to verify DSI communication...\n");

    // Clear error flags
    DSI->ISR[0] = 0xFFFFFFFF;
    DSI->ISR[1] = 0xFFFFFFFF;

    // Dummy read to prime the response pipeline
    // The first read returns stale/empty data, so we discard it
    dprintf("  Doing dummy read to prime pipeline...\n");
    (void)dsi_dcs_read_1byte(0xDA);
    dprintf("  Dummy read complete.\n");

    // Now do the actual reads - responses should be aligned correctly
    // Panel ID values per ST BSP drivers:
    //   OTM8009A: ID1 (0xDA) = 0x40  (older F469-Discovery boards)
    //   NT35510:  ID2 (0xDB) = 0x80  (Rev B08+ F469-Discovery boards)

    // Read ID1 - OTM8009A returns 0x40 here
    dprintf("  Reading ID1 (0xDA)...\n");
    uint8_t id1 = dsi_dcs_read_1byte(0xDA);
    dprintf("    ID1: 0x%02X (OTM8009A = 0x40)\n", id1);

    // Read ID2 - NT35510 returns 0x80 here
    dprintf("  Reading ID2 (0xDB)...\n");
    uint8_t id2 = dsi_dcs_read_1byte(0xDB);
    dprintf("    ID2: 0x%02X (NT35510 = 0x80)\n", id2);

    // Read ID3
    dprintf("  Reading ID3 (0xDC)...\n");
    uint8_t id3 = dsi_dcs_read_1byte(0xDC);
    dprintf("    ID3: 0x%02X\n", id3);

    // Check ISR for errors
    dprintf("\n  DSI Status after reads:\n");
    dprintf("    ISR[0]: 0x%08lX\n", (unsigned long)DSI->ISR[0]);
    dprintf("    ISR[1]: 0x%08lX\n", (unsigned long)DSI->ISR[1]);

    // Verify IDs per ST BSP drivers:
    //   OTM8009A_ID = 0x40 in ID1 (0xDA)
    //   NT35510_ID  = 0x80 in ID2 (0xDB)
    bool is_nt35510 = (id2 == 0x80);
    bool is_otm8009a = (id1 == 0x40);
    bool any_response = (id1 != 0xFF || id2 != 0xFF || id3 != 0xFF);

    dprintf("\n=== Panel ID Results ===\n");
    dprintf("  ID1=0x%02X, ID2=0x%02X, ID3=0x%02X\n", id1, id2, id3);

    if (is_nt35510) {
        dprintf("  Detected: NT35510 (Rev B08+ board)\n");
    } else if (is_otm8009a) {
        dprintf("  Detected: OTM8009A (older board revision)\n");
    } else if (any_response) {
        dprintf("  Unknown panel - got responses but IDs don't match known controllers\n");
    } else {
        dprintf("  No valid response from panel (all 0xFF)\n");
    }

    // Verify LTDC and DSI status
    dprintf("\nStep 12: Final status check...\n");
    dprintf("Step 12: Verifying display status...\n");
    dprintf("  LTDC->GCR: 0x%08lX (enabled: %s)\n",
            (unsigned long)LTDC->GCR,
            (LTDC->GCR & LTDC_GCR_LTDCEN) ? "YES" : "NO");
    dprintf("  DSI->CR: 0x%08lX (enabled: %s)\n",
            (unsigned long)DSI->CR,
            (DSI->CR & DSI_CR_EN) ? "YES" : "NO");
    dprintf("  DSI->WCR: 0x%08lX (wrapper enabled: %s)\n",
            (unsigned long)DSI->WCR,
            (DSI->WCR & DSI_WCR_DSIEN) ? "YES" : "NO");
    dprintf("  DSI->WISR: 0x%08lX\n", (unsigned long)DSI->WISR);
    dprintf("  DSI->ISR[0]: 0x%08lX\n", (unsigned long)DSI->ISR[0]);
    dprintf("  DSI->ISR[1]: 0x%08lX\n", (unsigned long)DSI->ISR[1]);

    dprintf("\n=== Display should now show RED screen ===\n");

    return true;
}

__attribute__((used)) bool dsi_test_read_panel_id_old(void) {
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

    // Configure PLLSAIDIVR for 27.429MHz LTDC pixel clock per ST F469-Discovery
    // PLLSAIR output = 384MHz / 7 = 54.857MHz, then /2 = 27.429MHz
    RCC->DCKCFGR &= ~RCC_DCKCFGR_PLLSAIDIVR_Msk;
    RCC->DCKCFGR |= RCC_DCKCFGR_PLLSAIDIVR_0; // Divide by 2 (bits [17:16] = 01)

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
    // Configure DSI PLL per ST F469-Discovery reference (500MHz PHY)
    // Input: 8MHz HSE
    // IDF = 2: VCO in = 8MHz / 2 = 4MHz
    // NDIV = 125: VCO = 4MHz × 125 = 500MHz
    // ODF = 1: PHY = 500MHz / 1 = 500MHz
    // Lane byte clock = 500MHz / 8 = 62.5MHz

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

    // Note: DSI wrapper and host will be enabled AFTER LTDC initialization

    // CRITICAL: Configure TX Escape Clock Divider (required for command mode)
    // Lane byte clock = 62.5 MHz, target TX Escape clock ~15.625 MHz
    // Divider = 62.5MHz / 15.625MHz = 4
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
    // IDF=2, ODF=0 (÷1), NDIV=125, HSE=8000 kHz
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

    // Configure Protocol Configuration Register (PCR) for flow control
    // This is CRITICAL for read operations - enables BTA (Bus Turn Around)
    DSI->PCR = DSI_PCR_BTAE |    // Bus Turn Around Enable (required for reads!)
               DSI_PCR_ECCRXE |  // ECC reception enable
               DSI_PCR_CRCRXE |  // CRC reception enable
               DSI_PCR_ETRXE |   // EoTp reception enable
               DSI_PCR_ETTXE;    // EoTp transmission enable

    dprintf("  PCR configured for flow control (BTA enabled): 0x%08lX\n", (unsigned long)DSI->PCR);

    dprintf("Step 7: Configuring DSI Video Mode...\n");
    // Configure virtual channel and DPI color coding
    DSI->GVCIDR = 0; // Virtual channel ID 0

    // Configure DSI Video Mode for OTM8009A (800x480 landscape)
    // Timing parameters from ST reference
    uint32_t VSA = 1;   // OTM8009A_480X800_VSYNC
    uint32_t VBP = 15;  // OTM8009A_480X800_VBP
    uint32_t VFP = 16;  // OTM8009A_480X800_VFP
    uint32_t HSA = 2;   // OTM8009A_480X800_HSYNC
    uint32_t HBP = 34;  // OTM8009A_480X800_HBP
    uint32_t HFP = 34;  // OTM8009A_480X800_HFP
    uint32_t HACT = 800;
    uint32_t VACT = 480;

    // Lane byte clock = 62.5 MHz, LCD clock = 27.429 MHz
    uint32_t laneByteClk_kHz = 62500;
    uint32_t LcdClock = 27429;

    // Configure video mode packet size
    DSI->VPCR &= ~DSI_VPCR_VPSIZE;
    DSI->VPCR = HACT; // Packet size = horizontal active width

    // Configure video mode timings (converted to lane byte clock cycles)
    DSI->VHSACR = (HSA * laneByteClk_kHz) / LcdClock;
    DSI->VHBPCR = (HBP * laneByteClk_kHz) / LcdClock;
    DSI->VLCR = ((HACT + HSA + HBP + HFP) * laneByteClk_kHz) / LcdClock;

    DSI->VVSACR = VSA;
    DSI->VVBPCR = VBP;
    DSI->VVFPCR = VFP;
    DSI->VVACR = VACT;

    // Configure video mode type (burst mode)
    // VMT bits: 00=non-burst sync pulses, 01=non-burst sync events, 10=burst
    DSI->VMCR &= ~DSI_VMCR_VMT;
    DSI->VMCR |= (2U << 0); // Burst mode (VMT = 10b)

    // Enable LP command transmission in video mode during blanking periods
    DSI->VMCR |= DSI_VMCR_LPCE;        // Enable LP commands in video mode
    DSI->VMCR |= DSI_VMCR_LPHFPE;      // LP during horizontal front porch
    DSI->VMCR |= DSI_VMCR_LPHBPE;      // LP during horizontal back porch
    DSI->VMCR |= DSI_VMCR_LPVAE;       // LP during vertical active
    DSI->VMCR |= DSI_VMCR_LPVFPE;      // LP during vertical front porch
    DSI->VMCR |= DSI_VMCR_LPVBPE;      // LP during vertical back porch
    DSI->VMCR |= DSI_VMCR_LPVSAE;      // LP during vertical sync (corrected)

    // Set largest packet size for LP commands (using LPMCR register)
    DSI->LPMCR &= ~DSI_LPMCR_LPSIZE;
    DSI->LPMCR |= 16U << DSI_LPMCR_LPSIZE_Pos;  // 16 bytes max

    dprintf("  Video mode: %lux%lu, HSA=%lu, HBP=%lu, HFP=%lu, VSA=%lu, VBP=%lu, VFP=%lu\n",
            (unsigned long)HACT, (unsigned long)VACT,
            (unsigned long)HSA, (unsigned long)HBP, (unsigned long)HFP,
            (unsigned long)VSA, (unsigned long)VBP, (unsigned long)VFP);

    dprintf("Step 8: Initializing LTDC...\n");

    // Enable LTDC clock (already enabled in Step 1)
    // Configure LTDC timing (derived from DSI video mode)
    LTDC->SSCR = ((HSA - 1) << LTDC_SSCR_HSW_Pos) | ((VSA - 1) << LTDC_SSCR_VSH_Pos);
    LTDC->BPCR = ((HSA + HBP - 1) << LTDC_BPCR_AHBP_Pos) | ((VSA + VBP - 1) << LTDC_BPCR_AVBP_Pos);
    LTDC->AWCR = ((HACT + HSA + HBP - 1) << LTDC_AWCR_AAW_Pos) | ((VACT + VSA + VBP - 1) << LTDC_AWCR_AAH_Pos);
    LTDC->TWCR = ((HACT + HSA + HBP + HFP - 1) << LTDC_TWCR_TOTALW_Pos) | ((VACT + VSA + VBP + VFP - 1) << LTDC_TWCR_TOTALH_Pos);

    // Configure synchronization polarity
    LTDC->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);
    LTDC->GCR |= LTDC_GCR_PCPOL; // Pixel clock polarity

    // Set background color (black)
    LTDC->BCCR = 0x00000000;

    // Enable LTDC
    LTDC->GCR |= LTDC_GCR_LTDCEN;

    dprintf("  LTDC initialized: SSCR=0x%08lX, BPCR=0x%08lX, AWCR=0x%08lX, TWCR=0x%08lX\n",
            (unsigned long)LTDC->SSCR, (unsigned long)LTDC->BPCR,
            (unsigned long)LTDC->AWCR, (unsigned long)LTDC->TWCR);

    dprintf("Step 9: Starting DSI in video mode...\n");

    // Start DSI wrapper (critical: must be done AFTER LTDC init)
    DSI->WCR |= DSI_WCR_DSIEN;
    DSI->CR |= DSI_CR_EN;

    dprintf("  DSI started in video mode\n");

    // Wait for DSI to stabilize
    chThdSleepMilliseconds(20);

    dprintf("Step 10: Initializing OTM8009A panel...\n");

    // OTM8009A initialization sequence (simplified from ST reference)
    // Enable CMD2
    dprintf("  Enabling CMD2...\n");
    dsi_dcs_write_1param(0x00, 0x00);  // NOP
    const uint8_t cmd2_enable1[] = {0x80, 0x09, 0x01};
    dsi_generic_write(0xFF, cmd2_enable1, 3);

    dsi_dcs_write_1param(0x00, 0x80);  // Shift address
    const uint8_t cmd2_enable2[] = {0x80, 0x09};
    dsi_generic_write(0xFF, cmd2_enable2, 2);

    // Essential power and timing settings
    dsi_dcs_write_1param(0x00, 0x80);
    dsi_dcs_write_1param(0xC4, 0x30);
    chThdSleepMilliseconds(10);

    dsi_dcs_write_1param(0x00, 0x8A);
    dsi_dcs_write_1param(0xC4, 0x40);
    chThdSleepMilliseconds(10);

    dsi_dcs_write_1param(0x00, 0xB1);
    dsi_dcs_write_1param(0xC5, 0xA9);

    dsi_dcs_write_1param(0x00, 0x91);
    dsi_dcs_write_1param(0xC5, 0x34);

    dsi_dcs_write_1param(0x00, 0xB4);
    dsi_dcs_write_1param(0xC0, 0x50);

    dsi_dcs_write_1param(0xD9, 0x4E);

    // Exit CMD2
    dprintf("  Exiting CMD2...\n");
    dsi_dcs_write_1param(0x00, 0x00);
    const uint8_t cmd2_exit[] = {0xFF, 0xFF, 0xFF};
    dsi_generic_write(0xFF, cmd2_exit, 3);

    // Standard DCS commands
    dprintf("  Sending Sleep Out...\n");
    dsi_dcs_write_0param(0x11);  // Sleep Out
    chThdSleepMilliseconds(120);

    dprintf("  Setting pixel format to RGB888...\n");
    dsi_dcs_write_1param(0x3A, 0x77);  // COLMOD - RGB888

    dprintf("  Setting landscape orientation...\n");
    dsi_dcs_write_1param(0x36, 0x60);  // MADCTR - Landscape mode

    // Set column and page address for 800x480 landscape
    const uint8_t caset[] = {0x00, 0x00, 0x03, 0x1F};  // 0-799
    dsi_generic_write(0x2A, caset, 4);

    const uint8_t paset[] = {0x00, 0x00, 0x01, 0xDF};  // 0-479
    dsi_generic_write(0x2B, paset, 4);

    dprintf("  Enabling display...\n");
    dsi_dcs_write_0param(0x29);  // Display On

    dprintf("  Starting GRAM write...\n");
    dsi_dcs_write_0param(0x2C);  // Write Memory Start

    dprintf("OTM8009A initialization complete!\n");

    dprintf("Step 11: OLD FUNCTION - Reading panel IDs (for reference)...\n");

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
