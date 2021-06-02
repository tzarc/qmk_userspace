#!/bin/bash

set -e

abs_path() { python -c "import os,sys; print(os.path.realpath(sys.argv[1]))" "$1" ; }

this_script=$(abs_path "${BASH_SOURCE[@]}")
script_dir=$(dirname "$this_script")
qmk_firmware=$(abs_path "$script_dir/qmk_firmware")
qmk_builddir="$qmk_firmware/.build"

export PATH=/usr/lib/ccache:$PATH

[[ -d "$qmk_builddir" ]] || mkdir -p "$qmk_builddir"
time qmk multibuild -j$(( $(nproc) * 2 + 1 )) | tee "$qmk_builddir/build.log"
