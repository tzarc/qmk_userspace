#!/bin/bash
# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

umask 022

source `which env_parallel.bash`

set -eEuo pipefail
this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
build_dir="$script_dir/sha1_verification"

QMK_FIRMWARE_REPO=https://github.com/qmk/qmk_firmware.git
reproducible_build_flags="COMMAND_ENABLE=no SKIP_VERSION=yes"

errcho() { echo "$@" 1>&2; }
havecmd() { command command type "${1}" >/dev/null 2>&1 || return 1; }
usage() {
    errcho "Usage: $(basename "$this_script") --pr <PR number> [--branch <branch>]"
    errcho "  --pr <PR number>  The PR number to check"
    errcho "  --branch <branch> The branch to check against (default: develop)"
    exit 1
}

unset TARGET_PR_NUMBER
TARGET_BRANCH="develop"
while [[ -n "${1:-}" ]]; do
    case "$1" in
    --pr)
        TARGET_PR_NUMBER="$2"
        shift
        ;;
    --branch)
        TARGET_BRANCH="$2"
        shift
        ;;
    *) errcho "Unknown argument: $1" ;;
    esac
    shift
done

if [[ -z "${TARGET_PR_NUMBER:-}" ]]; then
    errcho "No PR number specified"
    errcho
    usage
    exit 1
fi

# Set up the build directory
[[ -d "$build_dir" ]] || mkdir -p "$build_dir"
cleanup() {
    errcho "Cleaning up..."
    cd "$script_dir"
    while [[ -n "$(mount | grep " $build_dir ")" ]]; do
        errcho "Waiting for $build_dir to unmount..."
        sleep 1
        sudo umount "$build_dir" || true
        sleep 1
    done
    sleep 1
    rm -rf --one-file-system "$build_dir"
}
trap cleanup EXIT

build_targets() {
    pushd "$pr_dir" >/dev/null 2>&1
    git diff --name-only $TARGET_BRANCH | sed -e 's@^keyboards/@@g' -e 's@/keymaps/.*$@@g' -e 's@/[^/]*$@@g' | sort | uniq
    popd >/dev/null 2>&1
}

build_one() {
    local target=$1
    echo $target
    [[ -d .build ]] || mkdir .build
    make ${target}:default $reproducible_build_flags 2>&1 | grep -v '⚠' > .build/$(echo $target | sed -e 's@/@_@g').log 2>&1
}

strip_calls() {
    cat - | sed -e 's@call\s\+0x[0-9a-fA-F]*@call 0x????@g' -e 's@jmp\s\+0x[0-9a-fA-F]*@jmp 0x????@g'
}

main() {
    sudo mount -t tmpfs tmpfs "$build_dir"
    cd "$build_dir"

    export base_dir="$build_dir/qmk_firmware_base"
    export pr_dir="$build_dir/qmk_firmware_pr"

    # Clone the base repo
    rsync -qaP "$script_dir/qmk_firmware/" "$base_dir/"
    cd "$base_dir"
    rm .git/hooks/*
    git remote set-url origin "$QMK_FIRMWARE_REPO"
    git checkout "$TARGET_BRANCH"
    git fetch origin "$TARGET_BRANCH"
    git pull --ff-only
    make git-submodule

    # Fetch the PR
    rsync -qaP --delete "$base_dir/" "$pr_dir/"
    cd "$pr_dir"
    git fetch origin pull/"$TARGET_PR_NUMBER"/head:pr/"$TARGET_PR_NUMBER"
    git checkout -b target_pr pr/"$TARGET_PR_NUMBER"
    git merge --no-edit --quiet $TARGET_BRANCH

    # Generate the list of targets
    targets=$(build_targets | xargs echo)

    # Build the base repo
    cd "$base_dir"
    make git-submodule
    env_parallel -j $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null) build_one ::: $targets
    { ls -1 *.hex *.bin *.uf2 2>/dev/null || true ; } | sort | xargs sha1sum > "$build_dir/sha1sums_base.txt"

    # Build the target PR repo
    cd "$pr_dir"
    make git-submodule
    env_parallel -j $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null) build_one ::: $targets
    { ls -1 *.hex *.bin *.uf2 2>/dev/null || true ; } | sort | xargs sha1sum > "$build_dir/sha1sums_pr.txt"

    # Work out the diff's between the two target builds
    diff -u "$build_dir/sha1sums_base.txt" "$build_dir/sha1sums_pr.txt" || true

    elf_files=$( (diff "$build_dir/sha1sums_base.txt" "$build_dir/sha1sums_pr.txt" || true) | awk '/(hex|bin|uf2)/ {print $3}' | sort | uniq | sed -e 's@\(hex\|bin\|uf2\)@elf@g' || true)

    objdump_params="-dS --no-addresses --no-show-raw-insn"
    for elf_file in $elf_files ; do
        if [[ -n "$(file "$pr_dir/.build/$elf_file" | grep AVR)" ]] ; then
            avr-objdump $objdump_params "$base_dir/.build/$elf_file" | strip_calls > "$build_dir/$elf_file.base.dis"
            avr-objdump $objdump_params "$pr_dir/.build/$elf_file" | strip_calls > "$build_dir/$elf_file.pr.dis"
        else
            arm-none-eabi-objdump $objdump_params "$base_dir/.build/$elf_file" | strip_calls > "$build_dir/$elf_file.base.dis"
            arm-none-eabi-objdump $objdump_params "$pr_dir/.build/$elf_file" | strip_calls > "$build_dir/$elf_file.pr.dis"
        fi
    done

    # Testing!
    bash
}

########################################################################################################################
########################################################################################################################
########################################################################################################################
########################################################################################################################
main
