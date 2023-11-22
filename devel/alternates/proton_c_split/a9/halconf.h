#pragma once

#define HAL_USE_PWM TRUE

#ifdef SERIAL_DRIVER_USART
#define HAL_USE_SERIAL TRUE
#endif  // SERIAL_DRIVER_USART

#ifdef SERIAL_DRIVER_USART_DUPLEX
#define HAL_USE_UART TRUE
#define UART_USE_WAIT TRUE
#endif  // SERIAL_DRIVER_USART_DUPLEX

#include_next "halconf.h"
