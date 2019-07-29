#pragma once

/* I2C parameters -> 1MHz I2C @ 48MHz clock */
#define I2C_DRIVER I2CD1
#define I2C1_BANK GPIOA
#define I2C1_SCL 9
#define I2C1_SDA 10
#define I2C1_SCL_PAL_MODE 4
#define I2C1_SDA_PAL_MODE 4
#define I2C1_TIMINGR_PRESC 0x00U
#define I2C1_TIMINGR_SCLDEL 0x03U
#define I2C1_TIMINGR_SDADEL 0x00U
#define I2C1_TIMINGR_SCLH 0x0FU
#define I2C1_TIMINGR_SCLL 0x33U

#include "common_config.h"
