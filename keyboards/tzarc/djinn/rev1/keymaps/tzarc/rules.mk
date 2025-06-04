# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
mkfile_path := $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
include $(mkfile_path)/../../../rev2/keymaps/tzarc/rules.mk
