MOD_DIR ?= $(QMK_USERSPACE)/qmk_firmware-mods
MOD_LIB_PATH ?= $(MOD_DIR)/lib
MOD_QUANTUM_DIR ?= $(MOD_DIR)/quantum

RV32_VM_ENABLE ?= no
ifneq ($(strip $(RV32_VM_ENABLE)),no)
	OPT_DEFS += -DRV32_VM_ENABLE
	COMMON_VPATH += $(MOD_LIB_PATH)/mini-rv32ima/mini-rv32ima
endif
