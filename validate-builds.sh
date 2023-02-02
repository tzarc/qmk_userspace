#!/bin/bash
# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

umask 022

#set -vx
#set -o functrace

set -eEuo pipefail
this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_firmware_dir="$(realpath "$script_dir/qmk_firmware/")" # change this once moved to util
validation_output="$script_dir/validation-output"

source_branch="develop"
branch_under_test="remove-config_common_h"

export PATH=/home/nickb/gcc-arm/gcc-arm-none-eabi-8-2018-q4-major/bin:$PATH

reproducible_build_flags="COMMAND_ENABLE=no SKIP_VERSION=yes"

[[ -d "$validation_output" ]] || mkdir -p "$validation_output"
echo -n >"$validation_output/upgrade.log"

append_log() {
    cat - 2>&1 | sed -r 's/\x1B\[[0-9;]*[JKmsu]//g' >> "$validation_output/upgrade.log" 2>&1
}

required_keyboard_builds() {
    pushd "$qmk_firmware_dir" >/dev/null 2>&1

    qmk list-keyboards 2>/dev/null | shuf -n3 | sort | while read kb ; do echo "$kb:default" ; done

    popd >/dev/null 2>&1
}

container_make() {
    make $@ $reproducible_build_flags | grep -v '⚠'
    #RUNTIME=docker ./util/docker_cmd.sh make $@ $reproducible_build_flags | grep -v '⚠'
}

build_single() {
    local build_target=$1
    local build_stage=$2
    local extraflags=${3:-}

    container_make distclean 2>&1 | append_log

    # Work out what the basename of the target binary is going to be
    local binary_basename="$(container_make ${build_target}:dump_vars ${extraflags:-} 2>/dev/null | grep -P '^TARGET=' | cut -d'=' -f2)"

    # Keep a list of the make variables
    container_make ${build_target}:dump_vars ${extraflags:-} 2>&1 > "${validation_output}/${binary_basename}_${build_stage}_vars.txt"

    # Build the target and save to log
    { container_make -j$(nproc) ${build_target} ${extraflags:-} 2>&1 || true ; } | append_log

    # Copy out the build's cflags
    cat ".build/obj_${binary_basename}/cflags.txt" | sed -e 's/ /\n/g' > "$validation_output/${binary_basename}_${build_stage}_cflags.txt"

    # Copy out the build's '.build' directory
    [[ ! -d "$validation_output/${binary_basename}_${build_stage}_build" ]] \
        || rm -rf "$validation_output/${binary_basename}_${build_stage}_build"
    mv ".build" "$validation_output/${binary_basename}_${build_stage}_build"

    # Fixup the binary offsets in the file so that if they're relocated but don't change, we don't really care
    find "$validation_output/${binary_basename}_${build_stage}_build" -name '*.lst' -exec sed -i 's#/tmp/cc......\.s#/tmp/ccTEMPFL.s#g' '{}' +

    # Dump out the assembly of the target
    if [[ -e "${binary_basename}.bin" ]] || [[ -e "${binary_basename}.uf2" ]] ; then
        arm-none-eabi-objdump -S "$validation_output/${binary_basename}_${build_stage}_build/${binary_basename}.elf" \
            > "$validation_output/${binary_basename}_${build_stage}_asm.txt"
        sha1sum "${binary_basename}.bin" | awk '{print $1}'
    elif [[ -e "${binary_basename}.hex" ]] ; then
        avr-objdump -S "$validation_output/${binary_basename}_${build_stage}_build/${binary_basename}.elf" \
            > "$validation_output/${binary_basename}_${build_stage}_asm.txt"
        sha1sum "${binary_basename}.hex" | awk '{print $1}'
    else
        exit 1
    fi
}

validate_build() {
    local build_target=$1
    local extraflags=${2:-}

    pushd "$qmk_firmware_dir" >/dev/null 2>&1

    git clean -xfd >/dev/null 2>&1 || true
    git checkout -- . >/dev/null 2>&1
    git checkout "$source_branch" >/dev/null 2>&1
    local before="$(build_single "$build_target" before ${extraflags:-} 2>/dev/null | grep -v '⚠')"

    git clean -xfd >/dev/null 2>&1 || true
    git checkout -- . >/dev/null 2>&1
    git checkout "$branch_under_test" >/dev/null 2>&1
    local after="$(build_single "$build_target" after ${extraflags:-} 2>/dev/null | grep -v '⚠')"

    if [[ "$before" == "$after" ]] ; then
        printf '\e[1;32m%50s - %s\e[0m\n' "$build_target" "$before"
        local binary_basename="$(container_make $build_target:dump_vars ${extraflags:-} 2>/dev/null | grep -P '^TARGET=' | cut -d'=' -f2)"
        rm -rf "$validation_output/"${binary_basename}*
    else
        printf '\e[1;31m%50s - %s != %s\e[0m\n' "$build_target" "$before" "$after"
    fi

    popd >/dev/null 2>&1
}

[[ -d "$validation_output" ]] || mkdir -p "$validation_output"

main() {
    echo
    echo -e "\e[1;33mPerforming SHA hash verification between branches \e[1;35m$source_branch\e[1;33m and \e[1;35m$branch_under_test\e[1;33m:\e[0m"

    required_builds=$(required_keyboard_builds | sort | uniq)
    for build_target in $required_builds ; do
        validate_build $build_target
    done
}

export SIZE_REGRESSION_EXECUTING=1
########################################################################################################################################################################################################
########################################################################################################################################################################################################
########################################################################################################################################################################################################
########################################################################################################################################################################################################
########################################################################################################################################################################################################
########################################################################################################################################################################################################

main