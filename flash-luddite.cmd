@echo off
:: Copyright 2018-2022 Nick Brassel (@tzarc)
:: SPDX-License-Identifier: GPL-2.0-or-later
setlocal
set PATH=%PATH%;E:\QMK_MSYS\mingw64\bin
dfu-programmer.exe atmega32u4 erase --force
dfu-programmer.exe atmega32u4 flash --force 40percentclub_luddite_tzarc.hex
dfu-programmer.exe atmega32u4 reset
endlocal
