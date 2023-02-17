#!/bin/bash
# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

set -e

abs_path() { python3 -c "import os,sys; print(os.path.realpath(sys.argv[1]))" "$1" ; }

this_script=$(abs_path "${BASH_SOURCE[@]}")
script_dir=$(dirname "$this_script")
qmk_firmware="/qmk_firmware"
qmk_builddir="$qmk_firmware/.build"

FILTER=""

[[ -d "$qmk_builddir" ]] || mkdir -p "$qmk_builddir"
qmk config user.qmk_home="$qmk_firmware"
qmk clean -a
MAKEFLAGS="-O --no-print-directory" qmk mass-compile -j$(( 2 * $(nproc) + 1 )) $FILTER | tee "$script_dir/build.log"
MAKEFLAGS="-O --no-print-directory" qmk mass-compile -j$(( 2 * $(nproc) + 1 )) -km via $FILTER | tee -a "$script_dir/build.log"

exit 0

# Mods when running in --shell:
sed -i 's#FILTER=.*#FILTER="-f MCU=STM32F411 -f SPLIT_KEYBOARD=yes"#g' ../run_ci_build.sh
echo AUDIO_ENABLE=yes >> keyboards/tkw/grandiceps/rules.mk
rm ../.repo-hash ; ../entrypoint.sh ; cat ../index.html ; ls -1 .build/*.html
