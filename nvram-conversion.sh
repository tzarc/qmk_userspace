#!/bin/bash
# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later

umask 022

set -eEuo pipefail

this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_firmware_dir="$(realpath "$script_dir/qmk_firmware/")" # change this once moved to util

target_branch="nvram-refactor"

cd "$qmk_firmware_dir"

git checkout -f develop || true
git branch -D $target_branch || true
git checkout -f -b $target_branch develop

if [[ "$target_branch" != "$(git branch --show-current)" ]] ; then
    echo "Expected branch '$target_branch', was branch '$(git branch --show-current)'" >&2
    exit 1
fi

git fetch --all --tags --prune
git reset --hard upstream/develop
git clean -xfd || true
hub merge --squash --no-commit https://github.com/qmk/qmk_firmware/pull/16170

string_replace() {
    local from_text=$1
    local to_text=$2
    local location=${3:-.}
    pushd "$location"
    echo -- "-------------------------"
    git grep --name-only "$from_text" | sort | uniq | while read file ; do
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
    git grep --name-only "$from_func" | sort | uniq | while read file ; do
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
    git grep --name-only "$search_func" | sort | uniq | while read file ; do
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

convert_one eeprom_read_block nvram_read_block
block_reorder nvram_read_block
convert_one eeprom_write_block nvram_write_block
block_reorder nvram_write_block
convert_one eeprom_update_block nvram_update_block
block_reorder nvram_update_block

git mv drivers/eeprom drivers/nvram
git mv platforms/chibios/drivers/eeprom platforms/chibios/drivers/nvram

git reset -- platforms/eeprom.h
git checkout -- platforms/eeprom.h

#git mv quantum/eeconfig.c quantum/nvconfig.c
#git mv quantum/eeconfig.h quantum/nvconfig.h
#string_replace 'eeconfig\.' 'nvconfig.'
#string_replace 'eeconfig_' 'nvconfig_'
#string_replace 'EECONFIG_' 'NVCONFIG_'

git mv platforms/chibios/drivers/nvram/eeprom_flash_emulated.c platforms/chibios/drivers/nvram/nvram_emulated_flash.c
git mv platforms/chibios/drivers/nvram/eeprom_flash_emulated.h platforms/chibios/drivers/nvram/nvram_emulated_flash.h
git mv platforms/chibios/drivers/nvram/eeprom_flash_emulated_defs.h platforms/chibios/drivers/nvram/nvram_emulated_flash_defs.h
string_replace 'eeprom_stm32\.' 'nvram_emulated_flash.'
string_replace 'eeprom_flash_emulated\.' 'nvram_emulated_flash.'
string_replace 'EEPROM_STM32_FLASH_EMULATED' 'NVRAM_EMULATED_FLASH'

git mv platforms/chibios/drivers/nvram/eeprom_teensy.c platforms/chibios/drivers/nvram/nvram_kinetis_flexram.c
git mv platforms/chibios/drivers/nvram/eeprom_teensy.h platforms/chibios/drivers/nvram/nvram_kinetis_flexram.h
string_replace 'eeprom_teensy\.' 'nvram_kinetis_flexram.'
string_replace 'EEPROM_TEENSY' 'NVRAM_KINETIS_FLEXRAM'

git mv drivers/nvram/eeprom_driver.c platforms/nvram_common.c
git mv drivers/nvram/eeprom_driver.h platforms/nvram_common.h
string_replace 'eeprom_driver' 'nvram_driver'
string_replace 'EEPROM_CUSTOM' 'NVRAM_CUSTOM'
string_replace 'EEPROM_DRIVER' 'NVRAM_DRIVER'
string_replace 'DNVRAM_DRIVER' 'DNVRAM_COMMON'
string_replace 'defined(NVRAM_DRIVER)' 'defined(NVRAM_COMMON)'
string_replace 'ifdef NVRAM_DRIVER' 'ifdef NVRAM_COMMON'
string_replace 'nvram_driver\.h' 'nvram_common.h'
string_replace 'nvram_driver\.c' 'nvram_common.c'

git mv drivers/nvram/eeprom_transient.c drivers/nvram/nvram_transient.c
git mv drivers/nvram/eeprom_transient.h drivers/nvram/nvram_transient.h
string_replace 'eeprom_transient' 'nvram_transient'
string_replace 'EEPROM_TRANSIENT' 'NVRAM_TRANSIENT'

git mv drivers/nvram/eeprom_custom.c-template drivers/nvram/nvram_custom.c-template


git ls-files | grep 'eeprom_driver\.md' | while read file ; do
    git reset -- $file
    git checkout -- $file
    git mv $file $(dirname $file)/nvram_driver.md
done
# TODO: modify docs/nvram_driver.md

string_replace 'not a valid EEPROM driver' 'not a valid NVRAM driver'

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

string_replace 'stored in the NVRAM after' 'stored in the EEPROM after' docs
string_replace 'stored in the NVRAM of your controller' 'stored in the NVRAM of your keyboard' docs
string_replace 'shorten the life of your MCU\.' "shorten the life of your board's persistent storage." docs
string_replace 'sets the handedness setting in NVRAM\.' 'sets the handedness setting in NVRAM (requires internal MCU NVRAM to be used).'

git reset -- docs/ChangeLog docs/faq_misc.md docs/fuse.txt
git checkout -- docs/ChangeLog docs/faq_misc.md docs/fuse.txt

string_replace 'drivers/eeprom' 'drivers/nvram'
string_replace '_noeeprom' '_no_nvram'

string_replace '/eeprom$' '/nvram'

cat <<__EOF__ >> platforms/nvram_common.h

uint8_t nvram_read_u8(uint32_t addr);
uint16_t nvram_read_u16(uint32_t addr);
uint32_t nvram_read_u32(uint32_t addr);
void nvram_read_block(uint32_t addr, void *buf, uint32_t len);

void nvram_write_u8(uint32_t addr, uint8_t value);
void nvram_write_u16(uint32_t addr, uint16_t value);
void nvram_write_u32(uint32_t addr, uint32_t value);
void nvram_write_block(uint32_t addr, const void *buf, size_t len);

void nvram_update_u8(uint32_t addr, uint8_t value);
void nvram_update_u16(uint32_t addr, uint16_t value);
void nvram_update_u32(uint32_t addr, uint32_t value);
void nvram_update_block(uint32_t addr, const void *buf, size_t len);
__EOF__

git add -A
git diff --cached
