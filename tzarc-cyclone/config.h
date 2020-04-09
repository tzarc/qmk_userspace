/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

/* USB Device descriptor parameter */
#define VENDOR_ID 0xF055
#define PRODUCT_ID 0x4919
#define DEVICE_VER 0x0001
#define MANUFACTURER QMK
#define PRODUCT Cyclone
#define DESCRIPTION Cyclone Gamepad

/* matrix debugging */
#define DEBUG_MATRIX_SCAN_RATE

// STM32L082's have 6kB EEPROM
#define STM32_ONBOARD_EEPROM_SIZE (6 * 1024)
#define DYNAMIC_KEYMAP_MACRO_EEPROM_SIZE (STM32_ONBOARD_EEPROM_SIZE - DYNAMIC_KEYMAP_MACRO_EEPROM_ADDR)

/* key matrix size */
#define MATRIX_ROWS 8
#define MATRIX_COLS 8
#define DIODE_DIRECTION CUSTOM_MATRIX

/* Bootmagic lite definitions */
#define BOOTMAGIC_LITE_ROW 2
#define BOOTMAGIC_LITE_COLUMN 0

/* Shift register matrix pin allocations */
#define SHIFTREG_MATRIX_ROW_LATCH PIN_ROW_LATCH_3V3
#define SHIFTREG_MATRIX_ROW_CLK PIN_ROW_CLK_3V3
#define SHIFTREG_MATRIX_ROW_DATA PIN_ROW_DATA_3V3

#define SHIFTREG_MATRIX_COL_LATCH PIN_COL_LATCH_3V3
#define SHIFTREG_MATRIX_COL_CLK PIN_COL_CLK_3V3
#define SHIFTREG_MATRIX_COL_DATA PIN_COL_DATA_3V3

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 5

/* SPI Parameters */
#define SPI_DRIVER SPID1
#define SPI_SCK_PIN A5
#define SPI_MOSI_PIN A7
#define SPI_MISO_PIN A6
#define SPI_SCK_PAL_MODE 0
#define SPI_MOSI_PAL_MODE 0
#define SPI_MISO_PAL_MODE 0

#define EXTERNAL_EEPROM_SPI_CHIP_SELECT_PIN A0
#define EXTERNAL_EEPROM_SPI_CLOCK_DIVISOR 2

/* I2C parameters -> 1MHz I2C @ 32MHz clock */
#define I2C_DRIVER I2CD1
#define I2C1_BANK GPIOA
#define I2C1_SCL 9
#define I2C1_SDA 10
#define I2C1_SCL_PAL_MODE 6
#define I2C1_SDA_PAL_MODE 6
#define I2C1_TIMINGR_PRESC 0x00U
#define I2C1_TIMINGR_SCLDEL 0x01U
#define I2C1_TIMINGR_SDADEL 0x00U
#define I2C1_TIMINGR_SCLH 0x04U
#define I2C1_TIMINGR_SCLL 0x13U

/* GPIO pin allocations */
#define PIN_EN_5V_IO_INV C14
#define PIN_EN_5V_IO C15

#define PIN_ROW_LATCH_3V3 B4
#define PIN_ROW_CLK_3V3 B5
#define PIN_ROW_DATA_3V3 A3

#define PIN_COL_LATCH_3V3 A4
#define PIN_COL_CLK_3V3 B3
#define PIN_COL_DATA_3V3 A15

#define PIN_WS2812_3V3 A2

/* WS2812 configuration */
#ifdef WS2812
// Common
#    define RGB_DI_PIN PIN_WS2812_3V3
#    define RGBLED_NUM 15
// Bitbang
#    define NOP_FUDGE 0.5  // default of 0.4 gives flickering
// PWM
#    define WS2812_PWM_DRIVER PWMD2               // default: PWMD2
#    define WS2812_PWM_CHANNEL 3                  // default: 2
#    define WS2812_PWM_PAL_MODE 2                 // Pin "alternate function", see the respective datasheet for the appropriate values for your MCU. default: 2
#    define WS2812_DMA_STREAM STM32_DMA1_STREAM2  // DMA Stream for TIMx_UP, see the respective reference manual for the appropriate values for your MCU.
#    define WS2812_DMA_CHANNEL 8                  // DMA Channel for TIMx_UP, see the respective reference manual for the appropriate values for your MCU.
#endif                                            // WS2812

/* RGB backlighting configuration */
#ifdef RGBLIGHT_ENABLE
#    define RGBLIGHT_ANIMATIONS
#    define RGBLIGHT_LIMIT_VAL 160
#endif  // RGBLIGHT_ENABLE

/* RGB matrix configuration */
#ifdef RGB_MATRIX_ENABLE
#    define DRIVER_LED_TOTAL RGBLED_NUM
#    define RGB_MATRIX_MAXIMUM_BRIGHTNESS 160
#    define RGB_MATRIX_KEYPRESSES
#    define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#endif  // RGB_MATRIX_ENABLE

#define OLED_DISPLAY_128X64

#ifdef QWIIC_MICRO_OLED_ENABLE
#    define LCDWIDTH 128
#    define LCDHEIGHT 64
#    define I2C_ADDRESS_SA0_1 0x3C
#endif  // QWIIC_MICRO_OLED_ENABLE

#define JOYSTICK_BUTTON_COUNT 16
#define JOYSTICK_AXES_COUNT 0

#define IS_COMMAND() (matrix_is_on(6, 0) && matrix_is_on(7, 0) && matrix_is_on(7, 1) && matrix_is_on(7, 2))  // shift+ctrl+win+alt

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
//#define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION

/* ChibiOS hooks to reroute errors to QMK toolbox */
#define chDbgCheck(c)                                                                                   \
    do {                                                                                                \
        if (CH_DBG_ENABLE_CHECKS != FALSE) {                                                            \
            if (!(c)) {                                                                                 \
                extern void chibi_debug_check_hook(const char* func, const char* condition, int value); \
                chibi_debug_check_hook(__func__, (#c), (c));                                            \
                chSysHalt(__func__);                                                                    \
            }                                                                                           \
        }                                                                                               \
    } while (false)

#define chDbgAssert(c, r)                                                                                                    \
    do {                                                                                                                     \
        if (CH_DBG_ENABLE_ASSERTS != FALSE) {                                                                                \
            if (!(c)) {                                                                                                      \
                extern void chibi_debug_assert_hook(const char* func, const char* condition, int value, const char* reason); \
                chibi_debug_assert_hook(__func__, (#c), (c), (r));                                                           \
                chSysHalt(__func__);                                                                                         \
            }                                                                                                                \
        }                                                                                                                    \
    } while (false)
