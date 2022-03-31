@setlocal
@set PATH=%PATH%;e:\gcc-arm-none-eabi-10.3-2021.10\bin
@cd "%~dp0"
arm-none-eabi-gdb.exe -ex "cd qmk_firmware" -ex "target extended-remote COM6" -ex "monitor swdp_scan" -ex "monitor connect_srst enable" -ex "attach 1" -ex "set mem inaccessible-by-default off" -ex "load" -ex "compare-sections" -ex "enable breakpoint" -ex "break main" -ex "run" "qmk_firmware\\.build\\handwired_tzarc_ghoul_stm32_default.elf"
@endlocal
