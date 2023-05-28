#!/bin/bash
# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

umask 022

this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"

function searcher() {
    local search=$1
    local replace=$2
    pushd "$script_dir/qmk_firmware" >/dev/null
    find "lib/chibios" -name stm32_registry.h |
        while read file ; do
            grep -Po1 'defined\((STM32[^_][^\)]+)' "$file" |
                grep STM32 |
                grep -v 'F2XX' |
                grep -v 'MP' |
                grep -v '_' |
                cut -d'(' -f2 |
                sort |
                uniq |
                while read chip ; do
                    echo -e "#define $chip\n#include \"$file\"\n#ifndef $search\n#define $search FALSE\n#endif\n::: $search :::" |
                    arm-none-eabi-gcc -E - 2>&1 |
                    grep ':::' |
                    awk '{print $2}' |
                    xargs echo $(basename $(dirname $file)) $chip |
                    grep TRUE |
                    sed -e 's@TRUE@'$replace'@g' |
                    sort |
                    uniq
                done |
                sort |
                uniq
        done
    popd >/dev/null
}

{
    searcher STM32_HAS_USB USB
    searcher STM32_HAS_OTG1 OTG
    searcher STM32_HAS_OTG2 OTG
} | sort | uniq | column -t | tee "$script_dir/stm32_usb.txt"