#!/bin/bash
this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"

cd "$script_dir/qmk_firmware"
./util/chibios-upgrader.sh "$@"