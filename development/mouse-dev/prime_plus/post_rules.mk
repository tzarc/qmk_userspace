ifeq ($(strip $(QUANTUM_PAINTER_ENABLE)), yes)
	QUANTUM_PAINTER_DRIVERS = ld7032_i2c
	VPATH += keyboards/steelseries/prime_plus/graphics
	SRC += sharetech11.qff.c
	SRC += primeplus.qgf.c
else
	LIB_SRC += i2c_master.c
	SRC += ld7032.c
endif

ifeq ($(strip $(RGBLIGHT_ENABLE)), yes)
	SRC += rgblight_custom.c
endif
