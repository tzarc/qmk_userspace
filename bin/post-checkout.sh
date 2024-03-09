#!/bin/bash
# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

umask 022

set -eEuo pipefail

if [[ ! -z "${SIZE_REGRESSION_EXECUTING:-}" ]]; then
    # Skip executing if we're doing size regression analysis
    exit 0
fi

if [[ -z "$(which python3 | grep '/.direnv/')" ]]; then
    # Skip if we're not running python under direnv
    exit 0
fi

if [ "${1:-}" = "" -o "${1:-}" != "${2:-}" ]; then
    # Work out where we are
    this_script=$(realpath "${BASH_SOURCE[0]}")
    script_dir=$(dirname "$this_script")
    firmware_dir=$(realpath "$script_dir/../qmk_firmware")
    direnv_dir=$(realpath "$script_dir/../.direnv")

    # If we have a valid direnv and valid qmk_firmware, do the actual processing
    if [[ -d "$direnv_dir" ]] && [[ -d "$firmware_dir" ]]; then

        # Figure out and enter the qmk venv
        activation_script=$(ls -1 "$direnv_dir/"*"/bin/activate" | sort | head -n1)
        source "$activation_script"

        # Upgrade QMK CLI and all other deps
        python3 -m pip install --upgrade pip
        python3 -m pip install --upgrade -r "$script_dir/../python-requirements.txt"

        # Nuke all the git submodules that may or may not be present in different branches
        pushd "$firmware_dir" >/dev/null 2>&1 \
            && { [ -e lib/chibios-contrib/ext/mcux-sdk ] && rm -rf lib/chibios-contrib/ext/mcux-sdk || true ; } \
            && { [ -e lib/littlefs ] && rm -rf lib/littlefs || true ; } \
            && { [ -e lib/lua ] && rm -rf lib/lua || true ; } \
            && { [ -e lib/lvgl ] && rm -rf lib/lvgl || true ; } \
            && { [ -e lib/pico-sdk ] && rm -rf lib/pico-sdk || true ; } \
            && { [ -e lib/riot ] && rm -rf lib/riot || true ; } \
            && { [ -e lib/ugfx ] && rm -rf lib/ugfx || true ; } \
            && popd >/dev/null 2>&1

        # Reconfigure git submodules
        pushd "$firmware_dir" >/dev/null 2>&1 \
            && qmk git-submodule -f \
            && popd >/dev/null 2>&1

        # Drop out of the qmk venv
        deactivate
    fi
fi
