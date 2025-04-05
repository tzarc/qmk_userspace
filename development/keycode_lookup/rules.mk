# Copyright 2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

.PHONY: generated-files all test clean
generated-files: keycode_lookup.c

keycode_lookup.c: make_keycode_lookup.py
	@python make_keycode_lookup.py > keycode_lookup.c
