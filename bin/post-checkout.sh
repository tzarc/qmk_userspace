#!/bin/bash
# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

umask 022

set -eEuo pipefail

if [[ ! -z "${SIZE_REGRESSION_EXECUTING:-}" ]]; then
    # Skip executing if we're doing size regression analysis
    exit 0
fi

if [[ -z "$(which python3 | grep '/.venv/')" ]]; then
    # Skip if we're not running python under venv
    exit 0
fi

if [ "${1:-}" = "" -o "${1:-}" != "${2:-}" ]; then
    # Work out where we are
    this_script=$(realpath "${BASH_SOURCE[0]}")
    script_dir=$(dirname "$this_script")
    firmware_dir=$(realpath "$script_dir/../qmk_firmware")
    module_dir=$(realpath "$script_dir/../modules/tzarc")
    venv_dir=$(realpath "$script_dir/../.venv")

    # If we have a valid venv and valid qmk_firmware, do the actual processing
    if [[ -d "$venv_dir" ]] && [[ -d "$firmware_dir" ]]; then
        # Everything here happens within qmk_firmware
        cd "$firmware_dir"

        # Figure out and enter the qmk venv
        source "$venv_dir/bin/activate"

        # Ensure we have uv available
        [[ ! -z "$(which uv)" ]] || pip install uv

        # Upgrade QMK CLI and all other deps
        uv pip install --upgrade pip uv -r "$script_dir/../python-requirements.txt"

        # Install the pre-commit hooks
        hash -r
        pre-commit install
        pushd "$module_dir" >/dev/null 2>&1
        pre-commit install
        popd >/dev/null 2>&1

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
            if [[ "$remove" != "" ]] && [[ "$remove" != "." ]] && [[ "$remove" != ".." ]]; then
                this_dir=$(realpath "$firmware_dir/$remove")
                if [[ $(realpath "$firmware_dir") != $(realpath "$this_dir") ]] && [[ -f "$this_dir/.git" ]] && [[ ! -z $(cat "$this_dir/.git" | head -n1 | grep '^gitdir:' || true) ]]; then
                    echo -n "Removing ${remove}... " >&2
                    rm -rf "$this_dir"
                    echo "done." >&2
                fi
            fi
        done <<<"$all_submodules"

        # Reconfigure git submodules
        git -C "$firmware_dir" submodule update --jobs $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null) --init --recursive

        # Drop out of the qmk venv
        deactivate
    fi
fi
