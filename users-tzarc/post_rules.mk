# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
GAME_MODES_ENABLE ?= yes

ifeq ($(strip $(GAME_MODES_ENABLE)),yes)
	OPT_DEFS += -DGAME_MODES_ENABLE

	SRC += \
		tzarc_wow.c \
		tzarc_diablo3.c
endif

#OPT = 0