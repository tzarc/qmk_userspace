#!/bin/bash

umask 022

set -eEuo pipefail

this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_firmware_dir="$(realpath "$script_dir/qmk_firmware/")" # change this once moved to util

cd "$qmk_firmware_dir"

convert_one() {
    local from_func=$1
    local to_func=$2
    echo -- "-------------------------"
    git grep "$from_func" | cut -d: -f1 | sort | uniq | while read file ; do
        echo $file
        sed -i \
            -e "s@$from_func(\s*\(\|(\(void\|uint8_t\|uint16_t\|uint32_t\)\s*\*)\)@$to_func(EECONFIG_NVRAM_FILENAME, @g" \
            "$file"
    done
}

convert_one eeprom_read_byte nvram_read_u8
convert_one eeprom_write_byte nvram_write_u8
convert_one eeprom_update_byte nvram_update_u8

convert_one eeprom_read_word nvram_read_u16
convert_one eeprom_write_word nvram_write_u16
convert_one eeprom_update_word nvram_update_u16

convert_one eeprom_read_dword nvram_read_u32
convert_one eeprom_write_dword nvram_write_u32
convert_one eeprom_update_dword nvram_update_u32

# TODO: Need to swap order of function parameters here
convert_one eeprom_read_block nvram_read_block
convert_one eeprom_write_block nvram_write_block
convert_one eeprom_update_block nvram_update_block

git diff
