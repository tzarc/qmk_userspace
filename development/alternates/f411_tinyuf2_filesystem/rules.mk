FLASH_DRIVER = spi
FILESYSTEM_DRIVER = lfs_spi_flash

VPATH += \
		$(QMK_USERSPACE)/qmk_firmware-mods \
		$(QMK_USERSPACE)/qmk_firmware-mods/quantum \
		$(QMK_USERSPACE)/qmk_firmware-mods/quantum/filesystem

EEPROM_DRIVER = custom
SRC += eeprom_filesystem.c
VPATH += \
		$(QMK_USERSPACE)/qmk_firmware-mods/drivers/eeprom


include $(QMK_USERSPACE)/qmk_firmware-mods/builddefs/common_features_mod.mk

CONSOLE_ENABLE = yes
