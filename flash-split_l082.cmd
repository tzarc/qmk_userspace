@echo off
setlocal
set PATH=%PATH%;E:\QMK_MSYS\mingw64\bin
st-link_cli -q -c SWD -p qmk_firmware\.build\handwired_splittest_nucleo32_l082_default.hex -HardRst
endlocal
