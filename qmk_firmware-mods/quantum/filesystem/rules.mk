# Filesystem Configurables
VALID_FILESYSTEM_DRIVERS := lfs_spi_flash

ifeq ($(filter $(FILESYSTEM_DRIVER),$(VALID_FILESYSTEM_DRIVERS)),)
  $(call CATASTROPHIC_ERROR,Invalid FILESYSTEM_DRIVER,FILESYSTEM_DRIVER="$(FILESYSTEM_DRIVER)" is not a valid filesystem driver)
else
	OPT_DEFS += -DFILESYSTEM_ENABLE
	COMMON_VPATH += \
		$(MOD_QUANTUM_DIR)/filesystem

	ifeq ($(strip $(FILESYSTEM_DRIVER)),lfs_spi_flash)
		FLASH_DRIVER = spi
		SPI_DRIVER_REQUIRED = yes
		COMMON_VPATH += \
			$(MOD_LIB_PATH)/littlefs
		SRC += \
			lfs.c \
			lfs_util.c \
			fs_lfs_common.c \
			fs_lfs_spi_flash.c
		OPT_DEFS += -DLFS_NO_MALLOC -DLFS_THREADSAFE -DLFS_NAME_MAX=40
	endif
endif
