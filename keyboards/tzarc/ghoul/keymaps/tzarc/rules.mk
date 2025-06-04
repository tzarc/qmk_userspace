# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

NKRO_ENABLE = yes
ENCODER_MAP_ENABLE = yes
#DYNAMIC_KEYMAP_ENABLE = yes
DEBUG_MATRIX_SCAN_RATE_ENABLE = yes
WPM_ENABLE = yes
SRC += ui.c

XAP_ENABLE = yes

KEYBOARD_SHARED_EP = yes
MOUSE_SHARED_EP = yes

#QUANTUM_PAINTER_DRIVERS += surface

#NVM_DRIVER = custom
#FILESYSTEM_DRIVER = lfs_flash
