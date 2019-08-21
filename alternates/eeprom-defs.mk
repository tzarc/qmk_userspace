ifeq ($(EEPROM_EXTERNAL_I2C), yes)
    TMK_COMMON_SRC += $(CHIBIOS_CONTRIB)/os/hal/src/hal_eeprom.c
    TMK_COMMON_SRC += $(CHIBIOS_CONTRIB)/os/hal/src/hal_ee24xx.c
    TMK_COMMON_DEFS += -DEEPROM_USE_EE24XX=1
    HALINC += $(CHIBIOS_CONTRIB)/os/hal/include
  else ifeq ($(EEPROM_EXTERNAL_SPI), yes)
    TMK_COMMON_SRC += $(CHIBIOS_CONTRIB)/os/hal/src/hal_eeprom.c
    TMK_COMMON_SRC += $(CHIBIOS_CONTRIB)/os/hal/src/hal_ee25xx.c
    TMK_COMMON_DEFS += -DEEPROM_USE_EE25XX=1
    HALINC += $(CHIBIOS_CONTRIB)/os/hal/include
  else 