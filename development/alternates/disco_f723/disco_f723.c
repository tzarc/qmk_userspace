// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <hal.h>
#include <stm32f723xx.h>
#include <quantum.h>

#if !defined(USB_HS_PHYC_TUNE_VALUE)
#    define USB_HS_PHYC_TUNE_VALUE 0x00000F13U /*!< Value of USB HS PHY Tune */
#endif                                         /* USB_HS_PHYC_TUNE_VALUE */

void keyboard_post_init_user(void) {
    uint32_t count = 0;
    while (++count < 2500000) {
        __NOP();
    }

    palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(12) | PAL_STM32_OSPEED_HIGHEST);

    // Init the UTMI interface
    USB_DRIVER.otg->GUSBCFG &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);
    USB_DRIVER.otg->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
    USB_DRIVER.otg->GUSBCFG &= ~USB_OTG_GUSBCFG_ULPI_UTMI_SEL;
    USB_DRIVER.otg->GCCFG |= USB_OTG_GCCFG_PHYHSEN;

    // Enable the clock for the PHYC
    RCC->APB2ENR |= RCC_APB2ENR_OTGPHYCEN;

    // Enable the USB_HS internal PHY
    USB_HS_PHYC->USB_HS_PHYC_LDO |= USB_HS_PHYC_LDO_ENABLE;
    // Wait for the LDO to become ready
    while ((USB_HS_PHYC->USB_HS_PHYC_LDO & USB_HS_PHYC_LDO_STATUS) == 0U) {
    }

    // Set the PHYC PLL input
    USB_HS_PHYC->USB_HS_PHYC_PLL &= ~(USB_HS_PHYC_PLL_PLLEN_Msk | USB_HS_PHYC_PLL_PLLSEL_Msk);
    switch (STM32_HSECLK) {
        case 12000000:
            USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_12MHZ;
            break;
        case 12500000:
            USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_12_5MHZ;
            break;
        case 16000000:
            USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_16MHZ;
            break;
        case 24000000:
            USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_24MHZ;
            break;
        case 25000000:
            USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_25MHZ;
            break;
        case 32000000:
            USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL1_PLLSEL_Msk;
            break; // Value not defined in header
    }

    // Enable PHY tuning
    USB_HS_PHYC->USB_HS_PHYC_TUNE |= USB_HS_PHYC_TUNE_VALUE;

    // Enable the PHY PLL
    USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL_PLLEN;
}
