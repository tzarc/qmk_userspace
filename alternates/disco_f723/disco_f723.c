// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <hal.h>
#include <stm32f723xx.h>
#include <quantum.h>

#if !defined(USB_HS_PHYC_TUNE_VALUE)
#    define USB_HS_PHYC_TUNE_VALUE 0x00000F13U /*!< Value of USB HS PHY Tune */
#endif                                         /* USB_HS_PHYC_TUNE_VALUE */

void board_init(void) {
    uint32_t count = 0;
    while (++count < 2500000) {
        __NOP();
    }

    palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST);

    // Enable the clock
    RCC->APB2ENR |= RCC_APB2ENR_OTGPHYCEN;

    // Enable the USB_HS internal PHY
    USB_HS_PHYC->USB_HS_PHYC_LDO |= USB_HS_PHYC_LDO_ENABLE;

    // Wait for the LDO to become ready
    count = 0;
    while ((USB_HS_PHYC->USB_HS_PHYC_LDO & USB_HS_PHYC_LDO_STATUS) == 0U) {
        if (++count > 250000) {
            break;
        }
        // chSysPolledDelayX(10);
    }

    // Enable PLL for 25MHz HSE
    USB_HS_PHYC->USB_HS_PHYC_PLL = (0x5U << 1);

    // Enable PHY tuning
    USB_HS_PHYC->USB_HS_PHYC_TUNE |= USB_HS_PHYC_TUNE_VALUE;

    // Enable the PHY PLL
    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL_PLLEN;
}
