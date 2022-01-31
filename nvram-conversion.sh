#!/bin/bash

umask 022

set -eEuo pipefail

this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_firmware_dir="$(realpath "$script_dir/qmk_firmware/")" # change this once moved to util

cd "$qmk_firmware_dir"

string_replace() {
    local from_text=$1
    local to_text=$2
    local location=${3:-.}
    pushd "$location"
    echo -- "-------------------------"
    git grep "$from_text" | cut -d: -f1 | sort | uniq | while read file ; do
        echo $file
        sed -i \
            -e "s@$from_text@$to_text@g" \
            "$file" || true
    done || true
    popd
}

convert_one() {
    local from_func=$1
    local to_func=$2
    echo -- "-------------------------"
    git grep "$from_func" | cut -d: -f1 | sort | uniq | while read file ; do
        echo $file
        sed -i \
            -e "s@$from_func(\s*@$to_func(@g" \
            -e "s@$to_func(\(.*\)\(void\|uint8_t\|uint16_t\|uint32_t\)\s*\*\s*addr@$to_func(\1uint32_t addr@g" \
            -e "s@$to_func(\(.*\)const\s*uint32_t\s*addr@$to_func(\1uint32_t addr@g" \
            -e "s@$to_func(\s*(\s*uint8_t\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*uint16_t\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*uint32_t\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*void\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*const\s*uint8_t\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*const\s*uint16_t\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*const\s*uint32_t\s*\*\s*)\s*@$to_func(@g" \
            -e "s@$to_func(\s*(\s*const\s*void\s*\*\s*)\s*@$to_func(@g" \
            "$file" || true
    done || true
}

block_reorder() {
    local search_func=$1
    echo -- "-------------------------"
    git grep "$search_func" | cut -d: -f1 | sort | uniq | while read file ; do
        echo $file
        sed -i \
            -e "s@$search_func(\s*\\(.*\\)\s*,\s*\\(.*\\)\s*,\s*\\(.*\\)\s*)@$search_func(\2, \1, \3)@g" \
            -e "s@$search_func(\s*(\s*void\s*\*\s*)\s*@$search_func(@g" \
            -e "s@$search_func(\s*(\s*uintptr_t\s*)\s*@$search_func(@g" \
            -e "s@$search_func(\s*(\s*const\s*void\s*\*\s*)\s*@$search_func(@g" \
            "$file" || true
    done || true
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

block_reorder nvram_read_block
block_reorder nvram_write_block
block_reorder nvram_update_block

git mv drivers/eeprom drivers/nvram
git mv platforms/chibios/drivers/eeprom platforms/chibios/drivers/nvram
git mv platforms/chibios/eeprom_* platforms/chibios/drivers/nvram

git mv platforms/chibios/drivers/nvram/eeprom_stm32.c platforms/chibios/drivers/nvram/nvram_emulated_flash.c
git mv platforms/chibios/drivers/nvram/eeprom_stm32.h platforms/chibios/drivers/nvram/nvram_emulated_flash.h
git mv platforms/chibios/drivers/nvram/eeprom_stm32_defs.h platforms/chibios/drivers/nvram/nvram_emulated_flash_defs.h

git mv drivers/nvram/eeprom_driver.c drivers/nvram/nvram_driver.c
git mv drivers/nvram/eeprom_driver.h drivers/nvram/nvram_driver.h
git mv drivers/nvram/eeprom_transient.c drivers/nvram/nvram_transient.c
git mv drivers/nvram/eeprom_transient.h drivers/nvram/nvram_transient.h
git mv drivers/nvram/eeprom_custom.c-template drivers/nvram/nvram_custom.c-template

git mv docs/eeprom_driver.md docs/nvram_driver.md

string_replace 'EEPROM' 'NVRAM' docs
string_replace 'eeprom-storage' 'nvram-storage' docs
string_replace 'I2C NVRAM' 'I2C EEPROM' docs
string_replace 'I2C-based NVRAM' 'I2C-based EEPROM' docs
string_replace 'SPI NVRAM' 'SPI EEPROM' docs
string_replace 'SPI-based NVRAM' 'SPI-based EEPROM' docs
string_replace 'NONVRAM' 'noeeprom' docs
string_replace 'QK_CLEAR_NVRAM' 'QK_CLEAR_EEPROM' docs
string_replace 'NVRAM_DRIVER' 'EEPROM_DRIVER' docs
string_replace 'EXTERNAL_NVRAM' 'EXTERNAL_EEPROM' docs
string_replace 'internal NVRAM' 'internal EEPROM' docs
string_replace 'these NVRAM files' 'these EEPROM files' docs
string_replace 'true NVRAM' 'true EEPROM' docs
string_replace 'Reset NVRAM' 'Reset EEPROM' docs
string_replace 'NVRAM chips' 'EEPROM chips' docs
git reset -- docs/ChangeLog
git checkout -- docs/ChangeLog
git checkout -- docs/fuse.txt

string_replace 'eeprom_stm32\.' 'eeprom_emulated_flash.'
string_replace 'eeprom_driver' 'nvram_driver'
string_replace 'eeprom_transient' 'nvram_transient'
string_replace 'drivers/eeprom' 'drivers/nvram'

git diff
