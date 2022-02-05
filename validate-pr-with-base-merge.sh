#!/bin/bash

# Copyright 2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

set -eEuo pipefail

this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_firmware_dir="$(realpath "$script_dir/qmk_firmware/")" # change this once moved to util

function usage() {
    echo "Usage:"
    echo "       $(basename "$0") [-b <Branch>] <PR Number>"
    echo "       $(basename "$0") -h"
    echo
    echo "    -h           : Shows this usage page."
    echo "    -b <Branch>  : The base branch to use when merging"
    echo "    <PR Number>  : The number of the PR to attempt to merge"
    exit 1
}

if [[ ${#} -eq 0 ]]; then
    usage
    exit 0
fi

base_branch="master"

while getopts "hb:" opt "$@" ; do
    case "$opt" in
        h) usage; exit 0;;
        b) base_branch="${OPTARG:-}";;
        \?) usage >&2; exit 1;;
    esac
done

shift $((OPTIND-1))
pr_number=$1

cd "$qmk_firmware_dir"

hub checkout -f $base_branch
git clean -xfd || true
git pull --ff-only || true

old_keyboards=$(qmk list-keyboards)

hub branch -D test-merge-$pr_number-to-$base_branch || true
hub checkout -b test-merge-$pr_number-to-$base_branch $base_branch
hub merge https://github.com/qmk/qmk_firmware/pull/$pr_number

if [[ -n "$(git status -s)" ]] ; then
    echo "There are still changes after merge, likely CRLF issues."
    git status
    exit 1
fi

new_keyboards=$(qmk list-keyboards)

function build_keyboard() {
    qmk compile -kb $1 -km all
}

function keyboards_to_build() {
    {
        git diff --name-only $base_branch | grep -v 'keymaps' | grep -E '^keyboards/' | grep -E 'rules.mk$' | sed -e 's@^keyboards/@@g' -e 's@/rules.mk$@@g'
        diff <(echo "$old_keyboards") <(echo "$new_keyboards") | awk '/>/ {print $2}'
    } | sort | uniq
}

keyboards_to_build | while IFS= read -r kb ; do
    build_keyboard $kb
done
