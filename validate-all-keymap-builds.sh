#!/bin/bash

umask 022

set -eEuo pipefail
this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_firmware_dir="$(realpath "$script_dir/qmk_firmware/")" # change this once moved to util
validation_output="$script_dir/validation-output"

source_branch="develop-with-env"
branch_under_test="dynamic-keymap-maxlen"
keymap_name="via"

[[ -d "$validation_output" ]] || mkdir -p "$validation_output"
echo -n >"$validation_output/upgrade.log"

append_log() {
    cat - | sed -r 's/\x1B\[[0-9;]*[JKmsu]//g' >> "$validation_output/upgrade.log"
}

run_once() {
    local test_branch="$1"

    pushd "$qmk_firmware_dir" >/dev/null 2>&1

    git clean -xfd >/dev/null 2>&1 || true
    git checkout -- . >/dev/null 2>&1 || true
    git checkout "$test_branch" >/dev/null 2>&1 || true

    if [[ "$test_branch" != "$(git branch --show-current)" ]] ; then
        echo "Expected branch '$test_branch', was branch '$(git branch --show-current)'" >&2
        exit 1
    fi

    qmk multibuild -j$(nproc) -km $keymap_name -e SKIP_GIT=yes -e SKIP_VERSION=yes -e COMMAND_ENABLE=no || true
    qmk multibuild -j$(( $(nproc) / 4 )) -km $keymap_name -e SKIP_GIT=yes -e SKIP_VERSION=yes -e COMMAND_ENABLE=no || true

    [[ ! -d "$validation_output/.build-$test_branch" ]] || rm -rf "$validation_output/.build-$test_branch"
    mv .build "$validation_output/.build-$test_branch"
    git branch --show-current > "$validation_output/.build-$test_branch/branch.txt"

    { sha1sum $(find . -maxdepth 1 -name '*.bin' -or -name '*.hex' -or -name '*.uf2' 2>/dev/null | xargs echo) || true ; } | sort -k+2 > "$validation_output/$test_branch.sha1sum.txt"

    mkdir "$validation_output/.build-$test_branch/asm"

    find "$validation_output/.build-$test_branch" -name '*.elf' | while read file ; do
        if [[ -n "$(file "$file" | grep 'AVR')" ]] ; then
            avr-objdump -S "$file" > "$validation_output/.build-$test_branch/asm/$(basename "$file" .elf).asm.txt"
        elif [[ -n "$(file "$file" | grep 'ARM')" ]] ; then
            arm-none-eabi-objdump -S "$file" > "$validation_output/.build-$test_branch/asm/$(basename "$file" .elf).asm.txt"
        else
            echo "Unknown: $(file "$file")"
        fi
    done

    find "$validation_output/.build-$test_branch/asm" -name '*.asm.txt' | while read file ; do
        sed -iE \
            -e "s@build-$test_branch/@@g" \
            -e 's@[ \t][0-9a-f]\+:@xx:@g' \
            "$file"
    done

    find "$validation_output/.build-$test_branch" -name cflags.txt | while read file ; do sed -i -e 's/ /\n/g' "$file" ; done

    popd >/dev/null 2>&1
}

run_once "$source_branch"
run_once "$branch_under_test"
