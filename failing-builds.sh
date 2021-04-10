#!/bin/bash

set -eEuo pipefail

this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_dir="$(realpath "$script_dir/qmk_firmware")"

pushd "$qmk_dir"

make distclean
remake -j -O dc01/right:default
remake -j -O dc01/arrow:default
remake -j -O dc01/numpad:default
#remake -j -O handwired/dactyl_manuform/5x6_right_trackball:default
remake -j -O handwired/freoduo:default
remake -j -O helix/rev2/sc:default
remake -j -O keebio/iris/rev3:default
remake -j -O kinesis/kint36:default
remake -j -O terrazzo:default

popd