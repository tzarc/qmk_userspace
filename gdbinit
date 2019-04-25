target remote | openocd -c "gdb_port pipe" -s /usr/share/openocd/scripts -f stlink.cfg
symbol-file ./qmk_firmware/.build/tzarc_cyclone_default.elf
monitor reset
