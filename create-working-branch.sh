#!/bin/bash

set -eEuo pipefail

this_script="$(readlink -f "${BASH_SOURCE[0]}")"
script_dir="$(readlink -f "$(dirname "$this_script")")"

unset upgrade_chibios
unset upgrade_chibios_confs
#upgrade_chibios=1
#upgrade_chibios_confs=1

target_branch="generated-workarea"
if [ ! -z ${upgrade_chibios:-} ] ; then
target_branch="generated-chibios-master-upgrade"
fi

declare -a prs_to_apply
prs_to_apply+=(8256) # dump_vars
#prs_to_apply+=(6165) # ARM audio DAC/PWM change
prs_to_apply+=(8778) # Dual-bank bootloader
#prs_to_apply+=(8291) # GPT ARM backlight
#prs_to_apply+=(7959) # PWM ARM backlight
#prs_to_apply+=(8559) # V-USB Console
#prs_to_apply+=(8916) # qmk compiledb
prs_to_apply+=(8834) # raw usage id override
prs_to_apply+=(9160) # arm split docs

rm -f "$script_dir"/*.patch || true

hard_reset() {
    local repo_upstream=$1
    local repo_name=$2
    local repo_branch=${3:-master}
    git rebase --abort || true
    git merge --abort || true
    git clean -xfd
    git checkout -- .
    git reset --hard
    git remote set-url origin git@github.com:tzarc/$repo_name.git
    git remote set-url origin --push git@github.com:tzarc/$repo_name.git
    git remote set-url upstream https://github.com/$repo_upstream/$repo_name.git
    git remote set-url upstream --push git@github.com:tzarc/$repo_name.git
    git fetch --all --tags --prune
    git checkout -f $repo_branch
    git reset --hard upstream/$repo_branch
    git push origin $repo_branch
    git branch -D $target_branch || true
    git checkout -b $target_branch
    git reset --hard upstream/$repo_branch
}

upgrade-chibios() {
    pushd "$script_dir/qmk_firmware/lib/chibios"
    hard_reset ChibiOS ChibiOS stable_20.3.x
    git push origin $target_branch --set-upstream --force-with-lease
    popd

    pushd "$script_dir/qmk_firmware/lib/chibios-contrib"
    hard_reset ChibiOS ChibiOS-Contrib chibios-20.3.x
    git push origin $target_branch --set-upstream --force-with-lease
    popd

    pushd "$script_dir/qmk_firmware"
    git add lib/chibios lib/chibios-contrib
    git commit -m 'Upgrade ChibiOS to master'
    popd
}

upgrade-chibios-confs() {
    pushd "$script_dir"
    make links
    popd

    pushd "$script_dir/qmk_firmware"
    ./util/chibios-upgrader.sh
    clang-format-7 -i quantum/stm32/chconf.h
    clang-format-7 -i quantum/stm32/halconf.h
    clang-format-7 -i quantum/stm32/mcuconf.h
    popd

    pushd "$script_dir"
    make clean
    popd

    pushd "$script_dir/qmk_firmware"
    git add -A
    git commit -am 'Upgrade ChibiOS conf files'
    popd
}

pushd "$script_dir/qmk_firmware"
hard_reset qmk qmk_firmware
make git-submodule
sed -i 's#qmk/ChibiOS#tzarc/ChibiOS#g' .gitmodules
git add -A
git commit -am "Retarget '$target_branch' to point to personal ChibiOS repositories" || true
popd

pushd "$script_dir"
if [ ! -z ${upgrade_chibios:-} ] ; then
    upgrade-chibios
fi
if [ ! -z ${upgrade_chibios_confs:-} ] ; then
    upgrade-chibios-confs
fi
popd

pushd "$script_dir/qmk_firmware"
for pr in ${prs_to_apply[@]} ; do
    echo -e "\e[38;5;203mPR $pr\e[0m"
    hub merge https://github.com/qmk/qmk_firmware/pull/${pr}
    git commit --amend -m "Merge qmk_firmware upstream PR ${pr}"
done
git push origin $target_branch --set-upstream --force-with-lease
popd
