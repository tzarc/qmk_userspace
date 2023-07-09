@echo off
:: Copyright 2018-2023 Nick Brassel (@tzarc)
:: SPDX-License-Identifier: GPL-2.0-or-later
@setlocal
@set PATH=%PATH%;E:\stlink\stlink-1.7.0-x86_64-w64-mingw32\bin
st-flash write qmk_firmware\handwired_onekey_nucleo_h723zg_reset.bin 0x08000000
@endlocal
