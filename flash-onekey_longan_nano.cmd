@setlocal
@set PATH=%PATH%;E:\QMK_MSYS\mingw64\bin
dfu-util.exe -a 0 -d 28e9:0189 -s 0x08000000:leave -D qmk_firmware\handwired_onekey_sipeed_longan_nano_reset.bin
@endlocal
