#!/bin/bash

set -eEuo pipefail

# Script params
unset pr_num
unset target_branch
unset intended_sha1
while [[ -n "${1:-}" ]] ; do
    case "$1" in
        (--pr)
            shift
            pr_num="$1"
            ;;
        (--sha1)
            shift
            intended_sha1="$1"
            ;;
        (--target-branch)
            shift
            target_branch="$1"
            ;;
        (*)
            break
            ;;
    esac
    shift
done

# Check params
if [[ -z ${pr_num:-} ]] || [[ -z ${target_branch:-} ]] || [[ -z ${intended_sha1:-} ]] ; then
    echo "ERROR -- Usage: $0 --target-branch (master|develop) --pr <pr number> --sha1 <sha1>" >&2
    exit 1
fi

# Command executor with log output
pcmd() {
    echo -e "\e[38;5;203mExec:\e[38;5;131m $@\e[0m"
    "$@"
}

# Info helper
st_info() {
    echo -e "\e[38;5;44mInfo:\e[38;5;37m $@\e[0m"
}

# Exit code handling
export script_rc=1
_internal_cleanup() {
    cd /qmk_firmware

    st_info "Resetting '/qmk_firmware'"
    pcmd git reset --hard
    pcmd git checkout -- .
    pcmd git clean -xfd

    st_info "Removing historical submodules"
    [ -e lib/ugfx ] && pcmd rm -rf lib/ugfx
    [ -e lib/pico-sdk ] && pcmd rm -rf lib/pico-sdk
    [ -e lib/chibios-contrib/ext/mcux-sdk ] && pcmd rm -rf lib/chibios-contrib/ext/mcux-sdk

    st_info "Removing branch"
    pcmd git branch -D pr_${pr_num}
    pcmd git gc --auto

	exit $script_rc
}
trap _internal_cleanup EXIT HUP INT

main() {
    cd /qmk_firmware

    if [[ ! -d ".git" ]] ; then
        st_info "Cloning repository"
        pcmd git clone https://github.com/qmk/qmk_firmware.git /qmk_firmware
    fi

    st_info "Setting commit parameters"
    pcmd git config merge.renamelimit 200000
    pcmd git config user.name "Tzarc CI"
    pcmd git config user.email runner@ci.tzarc.org

    st_info "Fetching PR changes"
    pcmd git fetch origin pull/${pr_num}/head:pr_${pr_num}
    pcmd git log -n10 --oneline pr_${pr_num}
    pcmd git log -n1 --oneline ${intended_sha1}

    st_info "Resetting '/qmk_firmware'"
    pcmd git reset --hard
    pcmd git checkout -- .
    pcmd git clean -xfd

    st_info "Applying PR to $target_branch"
    pcmd git checkout $target_branch
    pcmd git merge --no-commit --squash ${intended_sha1}

    st_info "Preparing build"
    pcmd make git-submodule
    pcmd git status
}

main
script_rc=0