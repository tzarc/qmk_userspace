#!/bin/bash
set -eEuo pipefail

this_script="$(readlink -f "${BASH_SOURCE[0]}")"
script_dir="$(readlink -f "$(dirname "$this_script")")"

pushd "$script_dir/qmk_firmware"

make -j -O clueboard/2x1800/2019:default
make -j -O clueboard/66_hotswap/gen1:default
make -j -O kingly_keys/ave/ortho:default
make -j -O kingly_keys/ave/staggered:default
make -j -O lets_split/rev2:default
make -j -O lets_split/sockets:default
make -j -O lizard_trick/tenkey_plusplus:default
make -j -O lucid/kbd8x_hs:default
make -j -O mt64rgb:default
make -j -O mxss:default
make -j -O nightingale_studios/hailey:default
make -j -O nightly_boards/octopad:default
make -j -O squiggle/rev1:default
make -j -O the_royal/schwann:default
make -j -O treadstone32/rev1:default

popd
