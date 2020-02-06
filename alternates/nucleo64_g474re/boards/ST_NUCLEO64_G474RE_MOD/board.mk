# List of all the board related files.
BOARDSRC = $(BOARD_PATH)/boards/ST_NUCLEO64_G474RE_MOD/board.c

# Required include directories
BOARDINC = $(BOARD_PATH)/boards/ST_NUCLEO64_G474RE_MOD

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)
