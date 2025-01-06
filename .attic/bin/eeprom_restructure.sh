#!/bin/bash
# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

umask 022

set -eEuo pipefail

this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_firmware_dir="$(realpath "$script_dir/qmk_firmware/")" # change this once moved to util

target_branch="eeprom-restructure"

cd "$qmk_firmware_dir"

git fetch qmk develop
git checkout -f develop || true
git branch -D $target_branch || true
git checkout -f -b $target_branch qmk/develop

if [[ "$target_branch" != "$(git branch --show-current)" ]] ; then
    echo "Expected branch '$target_branch', was branch '$(git branch --show-current)'" >&2
    exit 1
fi

git cherry-pick 4f05584b01b17b88d620ddac5ae2527f8e482296 # Base set of common_features.mk changes

[[ -d platforms/chibios/drivers/flash/ ]] || mkdir -p platforms/chibios/drivers/flash/
git mv platforms/chibios/flash* platforms/chibios/drivers/flash/
git mv platforms/chibios/eeprom* platforms/chibios/drivers/eeprom/

git add -A
git commit -m 'File relocation.'
git fetch origin eeprom-restructure
git push --set-upstream origin eeprom-restructure --force-with-lease