# Copyright 2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

all: test

.PHONY: generated-files all test clean
generated-files: keycode_lookup.c

keycode_lookup: generated-files
	@gcc -DKEYCODE_TESTS -o keycode_lookup keycode_lookup.c

test: keycode_lookup
	@./keycode_lookup

clean:
	@rm -f keycode_lookup keycode_lookup.c

keycode_lookup.c: make_keycode_lookup.py keycode_lookup.c.j2
	@python3 make_keycode_lookup.py > keycode_lookup.c
