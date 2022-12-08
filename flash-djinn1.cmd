@echo off
:: Copyright 2018-2022 Nick Brassel (@tzarc)
:: SPDX-License-Identifier: GPL-2.0-or-later
@setlocal
@set PATH=%PATH%;E:\QMK_MSYS\mingw64\bin
dfu-util.exe -a 0 -d 0483:df11 -s 0x08000000:leave -D tzarc_djinn_rev1_tzarc.bin
@endlocal
