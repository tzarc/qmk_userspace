#!/bin/bash
# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later

set -eEuo pipefail

# Script params
unset target_repo
unset target_branch
unset pr_num
unset intended_sha1
while [[ -n "${1:-}" ]] ; do
    case "$1" in
        (--target-repo)
            shift
            target_repo="$1"
            ;;
        (--target-branch)
            shift
            target_branch="$1"
            ;;
        (--pr)
            shift
            pr_num="$1"
            ;;
        (--sha1)
            shift
            intended_sha1="$1"
            ;;
        (*)
            break
            ;;
    esac
    shift
done

# Check params
if [[ -z ${target_repo:-} ]] || [[ -z ${target_branch:-} ]] || [[ -z ${pr_num:-} ]] || [[ -z ${intended_sha1:-} ]] ; then
    echo "ERROR -- Usage: $0 --target-repo <url> --target-branch <branch> --pr <pr number> --sha1 <sha1>" >&2
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

git_hard_reset() {
    cd /__w/repo
    st_info "Resetting '/__w/repo'"
    pcmd git remote set-url origin $target_repo
    pcmd git fetch origin $target_branch
    pcmd git reset --hard
    pcmd git checkout -- .
    pcmd git clean -xfd
    pcmd git checkout $target_branch
    pcmd git reset --hard origin/$target_branch
    pcmd git checkout -- .
    pcmd git clean -xfd
}

main() {
    cd /__w/repo

    if [[ ! -d ".git" ]] ; then
        st_info "Cloning repository"
        pcmd git clone $target_repo /__w/repo
    fi

    st_info "Setting commit parameters"
    pcmd git config merge.renamelimit 200000
    pcmd git config user.name "Tzarc CI"
    pcmd git config user.email runner@ci.tzarc.org

    git_hard_reset

    st_info "Fetching PR #${pr_num} info"
    pcmd git fetch origin pull/${pr_num}/head:pr_${pr_num}

    st_info "Applying PR to $target_branch"
    pcmd git merge --no-commit --squash ${intended_sha1}

    st_info "Preparing build"
    pcmd make git-submodule
    pcmd git status -s
}

export script_rc=1
cleanup() {
    cd /__w/repo

    git_hard_reset

    st_info "Removing historical submodules"
    [ -e lib/ugfx ] && pcmd rm -rf lib/ugfx || true
    [ -e lib/pico-sdk ] && pcmd rm -rf lib/pico-sdk || true
    [ -e lib/chibios-contrib/ext/mcux-sdk ] && pcmd rm -rf lib/chibios-contrib/ext/mcux-sdk || true

    st_info "Removing branch"
    pcmd git branch -D pr_${pr_num}
    pcmd git gc --auto

	exit $script_rc
}
trap cleanup EXIT HUP INT

main
script_rc=0