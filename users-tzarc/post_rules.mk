GAME_MODES_ENABLE ?= yes

ifeq ($(strip $(GAME_MODES_ENABLE)),yes)
	OPT_DEFS += -DGAME_MODES_ENABLE

	SRC += \
		tzarc_wow.c \
		tzarc_diablo3.c
endif

