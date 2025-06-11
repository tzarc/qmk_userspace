#!/bin/bash
# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

umask 022

set -eEuo pipefail

if [[ -n ${SIZE_REGRESSION_EXECUTING:-} ]]; then
    # Skip executing if we're doing size regression analysis
    exit 0
fi

if [[ -z "$(which python3 | grep '/.venv/')" ]]; then
    # Skip if we're not running python under venv
    exit 0
fi

# Work out where we are
this_script=$(realpath "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")
firmware_dir=$(realpath "$script_dir/../qmk_firmware")
venv_dir=$(realpath "$script_dir/../.venv")

# If we have a valid venv and valid qmk_firmware, do the actual processing
if [[ -d $venv_dir ]] && [[ -d $firmware_dir ]]; then
    if [ "${1:-}" = "update" ]; then
        # Figure out and enter the qmk venv
        source "$venv_dir/bin/activate"

        pushd "$script_dir/../" >/dev/null 2>&1 &&
            pre-commit autoupdate &&
            popd >/dev/null 2>&1
        pushd "$script_dir/../modules/tzarc" >/dev/null 2>&1 &&
            pre-commit autoupdate &&
            popd >/dev/null 2>&1

        # Drop out of the qmk venv
        deactivate
    elif [ "${1:-}" = "" -o "${1:-}" != "${2:-}" ]; then # Either no-arguments, or args 1&2 are different (i.e. git checkout decided to change the working tree)
        # Everything here happens within qmk_firmware
        cd "$firmware_dir"

        # Figure out and enter the qmk venv
        source "$venv_dir/bin/activate"

        # Upgrade QMK CLI and all other deps
        uv pip install --upgrade pip -r "$script_dir/../python-requirements.txt"

        # Determine all the submodules we expect, as well as the ones we found on-disk
        actual_submodules=$(git -C "$firmware_dir" submodule --quiet foreach --recursive git rev-parse --show-toplevel | sed -e "s@${firmware_dir}/@@g")
        found_submodules=$(find lib -type f -name .git | while read p; do echo $(dirname $p); done)
        all_submodules=$(echo -e "$actual_submodules\n$found_submodules" | sort -u || true)
        while read actual; do
            # Remove the actual submodule from the total list of submodules
            all_submodules=$(echo "$all_submodules" | grep -vE "^$actual\$" || true)
        done <<<"$actual_submodules"

        # Remove any submodules that are no longer in use
        while read remove; do
            if [[ $remove != "" ]] && [[ $remove != "." ]] && [[ $remove != ".." ]]; then
                this_dir=$(realpath "$firmware_dir/$remove")
                if [[ $(realpath "$firmware_dir") != $(realpath "$this_dir") ]] && [[ -f "$this_dir/.git" ]] && [[ -n $(cat "$this_dir/.git" | head -n1 | grep '^gitdir:' || true) ]]; then
                    echo -n "Removing ${remove}... " >&2
                    rm -rf "$this_dir"
                    echo "done." >&2
                fi
            fi
        done <<<"$all_submodules"

        # Reconfigure git submodules
        git -C "$firmware_dir" submodule update --jobs $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null) --init --recursive

        # Install the git hooks
        hash -r
        pushd "$script_dir/../" >/dev/null 2>&1 &&
            pre-commit install -f &&
            popd >/dev/null 2>&1
        pushd "$script_dir/../modules/tzarc" >/dev/null 2>&1 &&
            pre-commit install -f &&
            popd >/dev/null 2>&1
        [[ -L "$firmware_dir/.git/hooks/post-checkout" ]] ||
            ln -sf "$script_dir/post-checkout.sh" "$firmware_dir/.git/hooks/post-checkout"

        # Drop out of the qmk venv
        deactivate
    fi
fi
