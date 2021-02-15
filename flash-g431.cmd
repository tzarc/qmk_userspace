@echo off
setlocal
set PATH=%PATH%;E:\QMK_MSYS\mingw64\bin
st-link_cli -q -c SWD -p qmk_firmware\.build\handwired_onekey_nucleo64_g431rb_reset.hex -HardRst
endlocal
