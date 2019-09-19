#!/bin/bash

set -eEuo pipefail

this_script="$(readlink -f "${BASH_SOURCE[0]}")"
script_dir="$(readlink -f "$(dirname "$this_script")")"

if [[ ! -L "$script_dir/qmk_firmware/keyboards/keebio/chocopad/keymaps/tzarc" ]] ; then
    ln -sf "$script_dir/tzarc-chocopad" "$script_dir/qmk_firmware/keyboards/keebio/chocopad/keymaps/tzarc"
fi

if [[ ! -L "$script_dir/qmk_firmware/keyboards/keebio/iris/keymaps/tzarc" ]] ; then
    ln -sf "$script_dir/tzarc-iris_rev4" "$script_dir/qmk_firmware/keyboards/keebio/iris/keymaps/tzarc"
fi

if [[ ! -L "$script_dir/qmk_firmware/keyboards/handwired/tzarc/cyclone" ]] ; then
    [[ -d "$script_dir/qmk_firmware/keyboards/handwired/tzarc" ]] \
        || mkdir -p "$script_dir/qmk_firmware/keyboards/handwired/tzarc"
    ln -sf "$script_dir/tzarc-cyclone" "$script_dir/qmk_firmware/keyboards/handwired/tzarc/cyclone"
fi
