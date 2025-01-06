#!/bin/bash
# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

umask 022

this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"

function searcher() {
    local search=$1
    local replace=$2
    pushd "$script_dir/qmk_firmware" >/dev/null
    find "lib/chibios" -name stm32_registry.h |
        while read file; do
            grep -Po1 'defined\((STM32[^_][^\)]+)' "$file" |
                grep STM32 |
                grep -v 'F2XX' |
                grep -v 'MP' |
                grep -v '_' |
                cut -d'(' -f2 |
                sort |
                uniq |
                while read chip; do
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

function all_targets() {
    {
        searcher STM32_HAS_USB USB
        searcher STM32_HAS_OTG1 OTG
        searcher STM32_HAS_OTG2 OTG
    } | sort | uniq | column -t
}

if [[ ! -f "$script_dir/stm32_usb.txt" ]]; then
    all_targets >"$script_dir/stm32_usb.txt"
fi

function peripheral_targets() {
    local peripheral_type="$1"
    local families=$(cat "$script_dir/stm32_usb.txt" | awk "/${peripheral_type}\$/ {print \$1}" | sort | uniq)
    for family in $(echo $families | sort | uniq); do
        local re_family=$(echo $family | sed -e 's@+@\\+@g')
        local chips=$(cat "$script_dir/stm32_usb.txt" | awk "/${peripheral_type}\$/" | awk "/^${re_family} / {print \$2}" | sort | uniq)
        for chip in $chips; do
            echo "$family: $chip"
        done
    done
}

function markdown_table() {
    # Accepts lines in format "family: chip" and transposes them into a table
    local -A families
    local -A chips
    while read line; do
        local family="$(echo "$line" | cut -d: -f1 | xargs echo)"
        local chip="$(echo "$line" | cut -d: -f2 | xargs echo)"
        families[$family]=1
        chips[$family]="${chips[$family]} $chip"
    done < <(cat -)

    local maxlen=0
    for family in "${!families[@]}"; do
        local thislen=$(echo ${chips[$family]} | wc -w)
        if [[ $maxlen -lt $thislen ]]; then maxlen=$thislen; fi
    done

    echo -n "|"
    for family in $(echo "${!families[@]}" | tr ' ' '\n' | sort | uniq); do echo -n " $family |"; done
    echo
    echo -n "|"
    for family in $(echo "${!families[@]}" | tr ' ' '\n' | sort | uniq); do echo -n " ---- |"; done
    echo
    for ((i = 1; i <= $maxlen; i++)); do
        echo -n "|"
        for family in $(echo "${!families[@]}" | tr ' ' '\n' | sort | uniq); do
            local thislen=$(echo ${chips[$family]} | wc -w)
            if [[ $thislen -lt $i ]]; then
                echo -n "      |"
                continue
            fi
            local chip=$(echo ${chips[$family]} | cut -d' ' -f$i)
            echo -n " $chip |"
        done
        echo
    done
}

{
echo "### USB:"
echo
peripheral_targets USB | markdown_table
echo
echo "### OTG:"
echo
peripheral_targets OTG | markdown_table
} | tee "$script_dir/stm32_usb.md"