#!/bin/bash

set -eEuo pipefail

this_script="$(readlink -f "${BASH_SOURCE[0]}")"
script_dir="$(readlink -f "$(dirname "$this_script")")"

declare -a prs_to_apply
prs_to_apply+=(8002) # STM32L0/L1 EEPROM
prs_to_apply+=(7928) # STM32 WS2812 PWM

rm -f "$script_dir"/*.patch || true

pushd "$script_dir/qmk_firmware"

git clean -xfd
git checkout -- .
git reset --hard
git fetch --all
git branch -D master || true
git checkout -f -b master upstream/master
git reset --hard upstream/master
git branch --force cyclone-merged-prs upstream/master
git checkout cyclone-merged-prs

for pr in ${prs_to_apply[@]} ; do
    echo -e "\e[38;5;203mPR $pr\e[0m"
    hub apply https://github.com/qmk/qmk_firmware/pull/${pr}
done

find . \( -name '*.rej' -or -name '*.orig' \) -delete

make git-submodule

git add -A
git commit -am 'Rebuild branch "cyclone-merged-prs"' || true
git push origin cyclone-merged-prs --set-upstream --force-with-lease

popd

upgrade-chibios() {
    pushd "$script_dir/qmk_firmware/lib/chibios"
    git remote set-url origin https://github.com/ChibiOS/ChibiOS.git
    git fetch --all
    git checkout master
    git reset --hard origin/master || true
    popd

    pushd "$script_dir/qmk_firmware/lib/chibios-contrib"
    git remote set-url origin https://github.com/ChibiOS/ChibiOS-Contrib.git
    git fetch --all
    git checkout master
    git reset --hard origin/master || true
    popd

    "$script_dir/links.sh"

    pushd "$script_dir/qmk_firmware"
    ./util/chibios-upgrader.sh

    git add -A
    git commit -am 'Upgrate ChibiOS conf files' || true
    git push origin cyclone-merged-prs --set-upstream --force-with-lease
    popd
}
