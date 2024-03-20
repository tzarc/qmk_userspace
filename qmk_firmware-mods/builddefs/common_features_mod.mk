MOD_DIR ?= $(QMK_USERSPACE)/qmk_firmware-mods
MOD_LIB_PATH ?= $(MOD_DIR)/lib
MOD_QUANTUM_DIR ?= $(MOD_DIR)/quantum

FILESYSTEM_DRIVER ?=
ifneq ($(strip $(FILESYSTEM_DRIVER)),)
    include $(MOD_QUANTUM_DIR)/filesystem/rules.mk
endif
