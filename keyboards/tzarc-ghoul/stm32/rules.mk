MCU = STM32F405
BOOTLOADER = stm32-dfu

WS2812_DRIVER = pwm

SRC += analog.c

OPT = g
OPT_DEFS += -g
