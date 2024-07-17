#!/usr/bin/env bash

this_script=$(realpath "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")
cd $script_dir/..

arm-none-eabi-gdb -nx --batch \
    -ex 'target extended-remote '$(ls -1 /dev/cu.usbmodem* | head -n1) \
    -ex 'monitor swdp_scan' \
    -ex 'attach 1' \
    -ex 'load' \
    -ex 'compare-sections' \
    -ex 'kill' \
    qmk_firmware/.build/handwired_onekey_f411_tinyuf2_filesystem_reset.elf
