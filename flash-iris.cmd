@echo off
setlocal
set PATH=%PATH%;E:\QMK_MSYS\mingw64\bin
dfu-programmer.exe atmega32u4 erase --force
dfu-programmer.exe atmega32u4 flash --force keebio_iris_rev4_tzarc.hex
dfu-programmer.exe atmega32u4 reset
endlocal
