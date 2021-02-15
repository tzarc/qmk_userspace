@echo off
setlocal
set PATH=%PATH%;E:\QMK_MSYS\mingw64\bin
st-link_cli -q -c SWD -p qmk_firmware\.build\handwired_tzarc_cyclone_default.hex -HardRst
endlocal
