#!/bin/bash

umask 022

target_branch="develop"
this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_firmware_dir="$(realpath "$script_dir/qmk_firmware/")" # change this once moved to util

cd "$qmk_firmware_dir"

spacer() {
    for f in $(seq 1 10) ; do
        echo
    done
}

reset_hadron() {
    git checkout develop -- keyboards/hadron/ver3/*conf.h
}

reset_hadron

qmk chibios-confmigrate -i keyboards/hadron/ver3/chconf.h -r platforms/chibios/common/configs/chconf.h -o
qmk chibios-confmigrate -i keyboards/hadron/ver3/halconf.h -r platforms/chibios/common/configs/halconf.h -o
qmk chibios-confmigrate -i keyboards/hadron/ver3/mcuconf.h -r platforms/chibios/GENERIC_STM32_F303XC/configs/mcuconf.h -o
make hadron/ver3:default

git st

reset_hadron
