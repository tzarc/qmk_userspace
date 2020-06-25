#include <quantum.h>
#include <raw_hid.h>
#include "tzarc.h"

#ifdef RAW_ENABLE

void raw_hid_receive(uint8_t *data, uint8_t length) { raw_hid_send((uint8_t *)&tzarc_eeprom_cfg, TZARC_EEPROM_ALLOCATION); }

#endif