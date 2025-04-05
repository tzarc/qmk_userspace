# Copyright 2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

all: test

include rules.mk

keycode_lookup: generated-files
	@gcc -DKEYCODE_TESTS -o keycode_lookup keycode_lookup.c

test: keycode_lookup
	@./keycode_lookup

clean:
	@rm -f keycode_lookup keycode_lookup.c
