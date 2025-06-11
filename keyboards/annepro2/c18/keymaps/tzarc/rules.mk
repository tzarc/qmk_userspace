# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

LTO_ENABLE = yes
OPT = s
XAP_ENABLE = no
RAW_ENABLE = no
CONSOLE_ENABLE = yes
MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes
MOUSE_SHARED_EP = yes
DEBUG_MATRIX_SCAN_RATE_ENABLE = no
NKRO_ENABLE = no
UNICODE_ENABLE = no
GAME_MODES_ENABLE = no
DIGITIZER_ENABLE = no
PROGRAMMABLE_BUTTON_ENABLE = no

generated-files: $(QMK_USERSPACE)/keyboards/annepro2/c18/keymaps/tzarc/undef_rgb_effects.h
$(QMK_USERSPACE)/keyboards/annepro2/c18/keymaps/tzarc/undef_rgb_effects.h: $(QMK_USERSPACE)/users/tzarc/enable_all_rgb_effects.h
	@cat $(QMK_USERSPACE)/users/tzarc/enable_all_rgb_effects.h \
		| sed -e 's@#\s*define@#undef@g' \
		| grep -vE '(SOLID|HUE)' \
		> $@
	@clang-format -i $@
