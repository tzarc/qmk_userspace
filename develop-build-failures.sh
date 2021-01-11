#!/bin/bash
set -eEuo pipefail

this_script="$(readlink -f "${BASH_SOURCE[0]}")"
script_dir="$(readlink -f "$(dirname "$this_script")")"

build_one() {
    pushd "$script_dir/qmk_firmware"
    remake -j$(( $(nproc) * 2 + 1 )) -O "$@"
    popd
}

build_one chavdai40/rev2:default
build_one ergodox_infinity:default
build_one infinity60:default
build_one k_type:default
build_one keebwerk/mega/ansi:default
build_one peiorisboards/ixora:default
build_one phoenix:default
build_one polilla/rev1:default
build_one tkw/stoutgat/v2/f411:default
build_one vinta:default
build_one whitefox:default
build_one zvecr/zv48/f401:default
build_one zvecr/zv48/f411:default
