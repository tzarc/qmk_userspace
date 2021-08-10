#!/bin/bash

set -e

abs_path() { python -c "import os,sys; print(os.path.realpath(sys.argv[1]))" "$1" ; }

this_script=$(abs_path "${BASH_SOURCE[@]}")
script_dir=$(dirname "$this_script")
qmk_firmware=$(abs_path "$script_dir/qmk_firmware")
qmk_builddir="$qmk_firmware/.build"

FILTER=""

[[ -d "$qmk_builddir" ]] || mkdir -p "$qmk_builddir"
qmk clean -a
MAKEFLAGS="-O --no-print-directory" qmk multibuild -j$(( $(nproc) + 1 )) $FILTER | tee "$script_dir/build.log"
MAKEFLAGS="-O --no-print-directory" qmk multibuild -j$(( $(nproc) + 1 )) -km via $FILTER | tee -a "$script_dir/build.log"

exit 0

# Mods when running in --shell:
sed -i 's#FILTER=.*#FILTER="-f MCU=STM32F411 -f SPLIT_KEYBOARD=yes"#g' ../run_ci_build.sh
echo AUDIO_ENABLE=yes >> keyboards/tkw/grandiceps/rules.mk
rm ../.repo-hash ; ../entrypoint.sh ; cat ../index.html ; ls -1 .build/*.html
