ifeq ($(strip $(PLATFORM_KEY)),chibios)
	XAP_ENABLE = no
else ifeq ($(strip $(PLATFORM_KEY)),arm_atsam)
	XAP_ENABLE = no
	LTO_ENABLE = no
else ifeq ($(strip $(PLATFORM_KEY)),avr)
	ifeq ($(strip $(PROTOCOL)),LUFA)
		# Uses defaults above
	else ifeq ($(strip $(PROTOCOL)),VUSB)
		CONSOLE_ENABLE = no
		XAP_ENABLE = no
		RAW_ENABLE = no
	endif
endif
