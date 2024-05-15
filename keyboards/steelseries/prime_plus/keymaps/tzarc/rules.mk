ENCODER_MAP_ENABLE = yes
DEBUG_MATRIX_SCAN_RATE_ENABLE = yes
CIE1931_CURVE = yes

MCU_LDSCRIPT = STM32F103xB_uf2boot

ifeq ($(wildcard drivers/painter/ld7032/qp_ld7032.c),)
	QUANTUM_PAINTER_ENABLE = no
endif
