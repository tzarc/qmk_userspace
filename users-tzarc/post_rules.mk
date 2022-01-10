ifeq ($(strip $(PLATFORM_KEY)),chibios)
	CREATE_MAP=yes
	EXTRAFLAGS=-fstack-usage
	EXTRALDFLAGS=-Wl,--print-memory-usage
else ifeq ($(strip $(PLATFORM_KEY)),arm_atsam)
	LTO_ENABLE = no
else ifeq ($(strip $(PLATFORM_KEY)),avr)
	ifeq ($(strip $(PROTOCOL)),LUFA)
		# Uses defaults as per rules.mk
	else ifeq ($(strip $(PROTOCOL)),VUSB)
		CONSOLE_ENABLE = no
		DEBUG_MATRIX_SCAN_RATE_ENABLE = no
	endif
endif
