#!/bin/bash

set -eEuo pipefail

this_script="$(readlink -f "${BASH_SOURCE[0]}")"
script_dir="$(readlink -f "$(dirname "$this_script")")"

declare -a prs_to_apply
prs_to_apply+=(8002) # STM32L0/L1 EEPROM
prs_to_apply+=(7928) # STM32 WS2812 PWM
prs_to_apply+=(8078) # ChibiOS timer fix

rm -f "$script_dir"/*.patch || true

pushd "$script_dir/qmk_firmware"

git clean -xfd
git checkout -- .
git reset --hard
git fetch --all
git branch -D future || true
git checkout -f -b future upstream/future
git reset --hard upstream/future
git branch --force cyclone-merged-prs upstream/future
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
