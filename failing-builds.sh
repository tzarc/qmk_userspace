#!/bin/bash

set -eEuo pipefail

this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_dir="$(realpath "$script_dir/qmk_firmware")"

pushd "$qmk_dir"

make distclean
remake -j -O draculad:default
remake -j -O handwired/dactyl_manuform/5x6_right_trackball:default

popd