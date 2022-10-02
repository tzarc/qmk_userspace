#!/bin/bash
# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later

this_script="$(readlink -f "${BASH_SOURCE[0]}")"
script_dir="$(readlink -f "$(dirname "$this_script")")"

docker rm qmk_develop_runner || true

rm "$script_dir/.repo-hash"
echo > "$script_dir/.repo-hash"
chmod 666 "$script_dir/.repo-hash"

renc -i "$HOME/.ssh/id_ed25519" < "$script_dir/create-builder.sh.enc" | bash