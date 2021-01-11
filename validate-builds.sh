#!/bin/bash

umask 022

set -eEuo pipefail
this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_firmware_dir="$(realpath "$script_dir/qmk_firmware/")" # change this once moved to util
validation_output="$script_dir/validation-output"

source_branch="develop"
branch_under_test="disable-all-chibios-subsystems"

export PATH=/home/nickb/gcc-arm/gcc-arm-none-eabi-8-2018-q4-major/bin:$PATH

[[ -d "$validation_output" ]] || mkdir -p "$validation_output"
echo -n >"$validation_output/upgrade.log"

append_log() {
    cat - | sed -r 's/\x1B\[[0-9;]*[JKmsu]//g' >> "$validation_output/upgrade.log"
}

build_single() {
    local build_target=$1
    local build_stage=$2
    local extraflags=${3:-}

    make distclean 2>&1 | append_log
    local binary_basename="$(make ${build_target}:dump_vars COMMAND_ENABLE=no SKIP_VERSION=yes SKIP_GIT=yes ${extraflags:-} | grep -P '^TARGET=' | cut -d'=' -f2)"
    make ${build_target}:dump_vars COMMAND_ENABLE=no SKIP_VERSION=yes SKIP_GIT=yes ${extraflags:-} 2>&1 > "${validation_output}/${binary_basename}_${build_stage}_vars.txt"
    { make -j$(nproc) ${build_target} COMMAND_ENABLE=no SKIP_VERSION=yes SKIP_GIT=yes ${extraflags:-} 2>&1 || true ; } | append_log
    cat ".build/obj_${binary_basename}/cflags.txt" | sed -e 's/ /\n/g' > "$validation_output/${binary_basename}_${build_stage}_cflags.txt"
    [[ ! -d "$validation_output/${binary_basename}_${build_stage}_build" ]] \
        || rm -rf "$validation_output/${binary_basename}_${build_stage}_build"
    mv ".build" "$validation_output/${binary_basename}_${build_stage}_build"
    find "$validation_output/${binary_basename}_${build_stage}_build" -name '*.lst' -exec sed -i 's#/tmp/cc......\.s#/tmp/ccTEMPFL.s#g' '{}' +
    if [[ -e "${binary_basename}.bin" ]] ; then
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

    git clean -xfd >/dev/null 2>&1
    git checkout -- . >/dev/null 2>&1
    git checkout "$source_branch" >/dev/null 2>&1
    local before="$(build_single "$build_target" before ${extraflags:-})"

    git clean -xfd >/dev/null 2>&1
    git checkout -- . >/dev/null 2>&1
    git checkout "$branch_under_test" >/dev/null 2>&1
    local after="$(build_single "$build_target" after ${extraflags:-})"

    if [[ "$before" == "$after" ]] ; then
        printf '\e[1;32m%50s - %s\e[0m\n' "$build_target" "$before"
        local binary_basename="$(make $build_target:dump_vars COMMAND_ENABLE=no SKIP_VERSION=yes SKIP_GIT=yes ${extraflags:-} | grep -P '^TARGET=' | cut -d'=' -f2)"
        rm -rf "$validation_output/"${binary_basename}*
    else
        printf '\e[1;31m%50s - %s != %s\e[0m\n' "$build_target" "$before" "$after"
    fi

    popd >/dev/null 2>&1
}

#required_keyboard_builds() {
#    pushd "$qmk_firmware_dir" >/dev/null 2>&1
#    git checkout "$source_branch" >/dev/null 2>&1
#
#    # All keyboards
#    find keyboards/ -type f -iname "rules.mk" | grep -v keymaps | sed 's!keyboards/\(.*\)/rules.mk!\1!' | sed -e 's@$@:default@g' | sort | uniq
#
#    #git grep '^\s*MCU\s*=\s*STM32F4' keyboards/ | grep -v keymaps | cut -d: -f1 | sed -e 's@^keyboards/@@g' -e 's@/rules.mk@:default@g'
#    #it grep '^\s*MCU\s*=\s*STM32F4' keyboards/ | grep keymaps | cut -d: -f1 | sed -e 's@^keyboards/@@g' -e 's@/keymaps/@:@g' -e 's@/rules.mk@@g'
#
#    popd >/dev/null 2>&1
#}

required_keyboard_builds() {
    #echo chavdai40/rev2:default
    #echo ergodox_infinity:default
    #echo infinity60:default
    #echo k_type:default
    #echo keebwerk/mega/ansi:default
    #echo peiorisboards/ixora:default
    #echo phoenix:default
    #echo polilla/rev1:default
    #echo tkw/stoutgat/v2/f411:default
    #echo vinta:default
    #echo whitefox:default
    echo zvecr/zv48/f401:default
    echo zvecr/zv48/f411:default
}

[[ -d "$validation_output" ]] || mkdir -p "$validation_output"

required_builds=$(required_keyboard_builds | sort | uniq)

for build_target in $required_builds ; do
    validate_build $build_target
done
