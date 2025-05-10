#!/usr/bin/env python3
# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
import lief

elf: lief.ELF.Binary = lief.ELF.parse("dynamic_lib.so")
elf = None
