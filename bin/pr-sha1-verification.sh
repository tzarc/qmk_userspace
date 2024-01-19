#!/bin/bash
# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

umask 022

set -eEuo pipefail
this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
userspace_dir="$(realpath "$script_dir/..")"

# Ensure we have access to the qmk venv if it exists
venv_activate_path=$(find $userspace_dir -path '*/.direnv/python*/bin/*' -name activate | head -n1)
if [[ ! -z "${venv_activate_path:-}" ]]; then
    echo "Activating environment: $venv_activate_path"
    source "$venv_activate_path"
fi

QMK_FIRMWARE_REPO=https://github.com/qmk/qmk_firmware.git
reproducible_build_flags="-e COMMAND_ENABLE=no -e SKIP_VERSION=yes -e KEEP_INTERMEDIATES=yes -e USE_CCACHE=no"

export RUNTIME=docker
export NCPUS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null)

errcho() { echo "$@" 1>&2; }
havecmd() { command command type "${1}" >/dev/null 2>&1 || return 1; }
usage() {
    errcho "Usage: $(basename "$this_script") --pr <PR number> [--branch <branch>]"
    errcho "  --pr <PR number>  The PR number to check"
    errcho "  --branch <branch> The branch to check against (default: develop)"
    exit 1
}

pcmd() {
    errcho -e "\e[38;5;203mExecuting:\e[38;5;131m $@\e[0m"
    "$@"
}

unset USE_RAMDISK
unset DOCKER_PREFIX
unset TARGET_PR_NUMBER
TARGET_BRANCH="develop"
while [[ -n "${1:-}" ]]; do
    case "$1" in
    --ramdisk)
        USE_RAMDISK=1
        ;;
    --pr)
        TARGET_PR_NUMBER="$2"
        shift
        ;;
    --branch)
        TARGET_BRANCH="$2"
        shift
        ;;
    --docker)
        DOCKER_PREFIX="./util/docker_cmd.sh"
        ;;
    -j)
        export NCPUS="$2"
        shift
        ;;
    *)
        errcho "Unknown argument: $1"
        ;;
    esac
    shift
done

if [[ -z "${TARGET_PR_NUMBER:-}" ]]; then
    errcho "No PR number specified"
    errcho
    usage
    exit 1
fi

build_dir=$(mktemp -d -p "$HOME/.local/" -t qmk.XXX)
# Set up the build directory
cleanup() {
    cd "$userspace_dir"
    if [[ -z "${FIRST_INIT:-}" ]]; then
        if [[ -d "$build_dir" ]]; then
            echo "Entering shell so that extra investigation can occur..."
            echo "Current directory: $(pwd)"
            echo "Verification directory: $build_dir"
            bash
        fi
    fi
    if [[ -n "${USE_RAMDISK:-}" ]]; then
        while [[ -n "$(mount | grep " $build_dir ")" ]]; do
            errcho "Waiting for $build_dir to unmount..."
            sleep 1
            sudo umount "$build_dir" || true
            sleep 1
        done
    fi
    if [[ -d "$build_dir" ]]; then
        sleep 1
        rm -rf --one-file-system "$build_dir"
    fi
}
trap cleanup EXIT INT TERM

build_targets() {
    pushd "$pr_dir" >/dev/null 2>&1

    # Auto-determined from modified files in diff, will traverse the children and pick out child boards too
    pcmd git diff --name-only $TARGET_BRANCH | grep -P '^keyboards' | sed -e 's@^keyboards/@@g' -e 's@/keymaps/.*$@@g' -e 's@/[^/]*$@@g' | while read kb; do
        find "keyboards/$kb" \( -name 'rules.mk' \) -and -not -path '*/keymaps/*' | while read rules; do
            kb=$(dirname "$rules" | sed -e 's@^keyboards/@@g')
            echo "${kb}:default"
        done
    done | sort | uniq

    # Generated from features present in boards
    #qmk find -f 'features.quantum_painter=true' | sort | uniq
    #qmk find -km apa102

    # Explicit board names
    #echo takashiski/namecard2x4/rev1:default takashiski/namecard2x4/rev2:default

    # Random selection of 100 of all boards
    #qmk find | shuf | head -n100

    popd >/dev/null 2>&1
}

build_one() {
    local target=$1
    echo $target
    [[ -d .build ]] || mkdir .build
    make ${target} $reproducible_build_flags 2>&1 | grep -v '⚠' >.build/$(echo $target | sed -e 's@/@_@g').log 2>&1
}

strip_calls() {
    cat - | sed \
        -e 's@call\s\+0x[0-9a-fA-F]\+@call 0x????@g' \
        -e 's@jmp\s\+0x[0-9a-fA-F]\+@jmp 0x????@g' \
        -e 's@lds\s\+r[0-9]\+,\s*0x[0-9a-fA-F]\+@lds r??, 0x????@g' \
        -e 's@sts\s\+0x[0-9a-fA-F]\+,\s*r[0-9]\+@sts 0x????, r??@g'
}

main() {
    export FIRST_INIT=1
    cleanup
    unset FIRST_INIT
    [[ -d "$build_dir" ]] || mkdir -p "$build_dir"
    if [[ -n "${USE_RAMDISK:-}" ]]; then
        sudo mount -t tmpfs tmpfs "$build_dir"
    fi
    cd "$build_dir"

    export base_dir="$build_dir/qmk_base"
    export pr_dir="$build_dir/qmk_pr"

    # Clone the base repo
    pcmd rsync -qaP --exclude='.build' "$userspace_dir/qmk_firmware/" "$base_dir/"
    cd "$base_dir"
    pcmd make distclean
    pcmd rm .git/hooks/*
    pcmd git remote set-url origin "$QMK_FIRMWARE_REPO"
    pcmd git checkout "$TARGET_BRANCH"
    pcmd git fetch origin "$TARGET_BRANCH"
    pcmd git clean -xfd
    pcmd git reset --hard
    pcmd git pull --ff-only
    pcmd make git-submodule

    # Fetch the PR
    pcmd rsync -qaP --delete "$base_dir/" "$pr_dir/"
    cd "$pr_dir"
    pcmd git fetch origin pull/"$TARGET_PR_NUMBER"/head:pr/"$TARGET_PR_NUMBER"
    pcmd git checkout -b target_pr pr/"$TARGET_PR_NUMBER"
    pcmd git merge --no-edit --quiet $TARGET_BRANCH

    # Generate the list of targets
    targets=$(build_targets | sort | uniq | xargs echo)

    if [[ -z "${targets:-}" ]]; then
        errcho "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
        errcho "@@ No targets found in diff! @@"
        errcho "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
        exit 1
    fi

    # Build the base repo
    cd "$base_dir"
    pcmd make git-submodule
    { pcmd ${DOCKER_PREFIX:-} qmk mass-compile -c -j $NCPUS $reproducible_build_flags $targets || true; } | tee "$build_dir/mass_compile_base.log"
    { ls -1 *.hex *.bin *.uf2 2>/dev/null || true; } | sort | xargs sha1sum >"$build_dir/sha1sums_base.txt"

    # Build the target PR repo
    cd "$pr_dir"
    pcmd make git-submodule
    { pcmd ${DOCKER_PREFIX:-} qmk mass-compile -c -j $NCPUS $reproducible_build_flags $targets || true; } | tee "$build_dir/mass_compile_pr.log"
    { ls -1 *.hex *.bin *.uf2 2>/dev/null || true; } | sort | xargs sha1sum >"$build_dir/sha1sums_pr.txt"

    local differences=$( (diff -yW 200 --suppress-common-lines "$build_dir/sha1sums_base.txt" "$build_dir/sha1sums_pr.txt" || true) | awk '{print $2}' | sed -e 's@\.\(hex\|bin\|uf2\)$@@g' | xargs echo)
    for difference in $differences; do
        { find "$base_dir/.build/obj_$difference" "$pr_dir/.build/obj_$difference" -type f -name '*.i' 2>/dev/null || true; } | while read file; do
            cat "$file" | sed -e 's@^#.*@@g' -e 's@^\s*//.*@@g' -e '/^\s*$/d' -e 's@qmk_base@qmk_xxx@g' -e 's@qmk_pr@qmk_xxx@g' | clang-format >"$file.formatted"
        done
    done

    echo
    echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
    echo '@@ Listing builds that failed on the base revision:           @@'
    echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
    { cat "$build_dir/mass_compile_base.log" | grep ERR; } || true

    echo
    echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
    echo '@@ Listing builds that failed on the PR:                      @@'
    echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
    { cat "$build_dir/mass_compile_pr.log" | grep ERR; } || true

    # Work out the diff's between the two target builds
    echo
    echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
    echo '@@ Listing builds that differ between the base and target PR: @@'
    echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
    echo '@@ Begin @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
    echo
    diff -yW 200 --suppress-common-lines "$build_dir/sha1sums_base.txt" "$build_dir/sha1sums_pr.txt" || true
    echo
    echo '@@ End @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'
    echo '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@'

    elf_files=$( (diff "$build_dir/sha1sums_base.txt" "$build_dir/sha1sums_pr.txt" || true) | awk '/(hex|bin|uf2)/ {print $3}' | sort | uniq | sed -e 's@\(hex\|bin\|uf2\)@elf@g' || true)

    objdump_params="-dS --no-addresses --no-show-raw-insn"
    for elf_file in $elf_files; do
        if [[ -n "$(file "$pr_dir/.build/$elf_file" | grep AVR)" ]]; then
            avr-objdump $objdump_params "$base_dir/.build/$elf_file" | strip_calls >"$build_dir/$elf_file.base.dis"
            avr-objdump $objdump_params "$pr_dir/.build/$elf_file" | strip_calls >"$build_dir/$elf_file.pr.dis"
        else
            arm-none-eabi-objdump $objdump_params "$base_dir/.build/$elf_file" | strip_calls >"$build_dir/$elf_file.base.dis"
            arm-none-eabi-objdump $objdump_params "$pr_dir/.build/$elf_file" | strip_calls >"$build_dir/$elf_file.pr.dis"
        fi
    done
}

########################################################################################################################
########################################################################################################################
########################################################################################################################
########################################################################################################################
main
