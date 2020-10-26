#!/bin/bash

set -eEuo pipefail

this_script="$(readlink -f "${BASH_SOURCE[0]}")"
script_dir="$(readlink -f "$(dirname "$this_script")")"

unset upgrade_chibios
unset upgrade_chibios_confs
upgrade_chibios=1
upgrade_chibios_confs=1

target_branch="generated-workarea"
target_qmk="develop"
if [ ! -z ${upgrade_chibios:-} ] ; then
target_branch="generated-chibios-master-upgrade"
target_qmk="develop"
fi

declare -a prs_to_apply
prs_to_apply+=(9603) # Matrix delay
prs_to_apply+=(10174) # Quantum Painter
prs_to_apply+=(10418) # ChibiOS conf upgrade
prs_to_apply+=(10437) # Decouple USB events
prs_to_apply+=(10730) # Last matrix activity

declare -a cherry_picks
cherry_picks+=(ba542a0164a10a1958aa5054d2a0cc333bb3ce64) # Audio

rm -f "$script_dir"/*.patch || true

pcmd() {
    echo -e "\e[38;5;203mExecuting:\e[38;5;131m $@\e[0m"
    "$@"
}

hard_reset() {
    local repo_upstream=$1
    local repo_name=$2
    local repo_branch=${3:-master}
    pcmd git rebase --abort || true
    pcmd git merge --abort || true
    pcmd git clean -xfd
    pcmd git checkout -- .
    pcmd git reset --hard
    pcmd git remote set-url origin git@github.com:tzarc/$repo_name.git
    pcmd git remote set-url origin --push git@github.com:tzarc/$repo_name.git
    pcmd git remote set-url upstream https://github.com/$repo_upstream/$repo_name.git
    pcmd git remote set-url upstream --push git@github.com:tzarc/$repo_name.git
    pcmd git fetch --all --tags --prune
    pcmd git fetch --unshallow upstream || true
    pcmd git checkout -f $repo_branch
    pcmd git reset --hard upstream/$repo_branch
    pcmd git push origin $repo_branch --force-with-lease
    pcmd git branch -D $target_branch || true
    pcmd git checkout -b $target_branch
    pcmd git reset --hard upstream/$repo_branch
}

upgrade-chibios() {
    pushd "$script_dir/qmk_firmware/lib/chibios"
    hard_reset ChibiOS ChibiOS stable_20.3.x
    pcmd git push origin $target_branch --set-upstream --force-with-lease
    popd

    pushd "$script_dir/qmk_firmware/lib/chibios-contrib"
    hard_reset ChibiOS ChibiOS-Contrib chibios-20.3.x
    pcmd git push origin $target_branch --set-upstream --force-with-lease
    popd

    pushd "$script_dir/qmk_firmware"
    pcmd git add lib/chibios lib/chibios-contrib
    pcmd git commit -m 'Upgrade ChibiOS to master'
    popd
}

upgrade-chibios-confs() {
    pushd "$script_dir/qmk_firmware"
    pcmd ./util/chibios-upgrader.sh
    popd

    pushd "$script_dir"
    pcmd make clean
    popd

    pushd "$script_dir/qmk_firmware"
    pcmd git add -A
    pcmd git commit -am 'Upgrade ChibiOS conf files'
    popd
}

pushd "$script_dir/qmk_firmware"
hard_reset qmk qmk_firmware $target_qmk
pcmd make git-submodule
pcmd sed -i 's#qmk/ChibiOS#tzarc/ChibiOS#g' .gitmodules
pcmd git add -A
pcmd git commit -am "Retarget '$target_branch' to point to personal ChibiOS repositories" || true
popd

pushd "$script_dir"
if [ ! -z ${upgrade_chibios:-} ] ; then
    upgrade-chibios
fi
popd

pushd "$script_dir/qmk_firmware"
for pr in ${prs_to_apply[@]} ; do
    echo -e "\e[38;5;203mPR $pr\e[0m"
    pcmd hub merge https://github.com/qmk/qmk_firmware/pull/${pr}
    pcmd git commit --amend -m "Merge qmk_firmware upstream PR ${pr}"
done
popd

pushd "$script_dir/qmk_firmware"
for hash in ${cherry_picks[@]} ; do
    echo -e "\e[38;5;203mCherry-picking $hash\e[0m"
    pcmd git cherry-pick $hash
done
popd

if [ ! -z ${upgrade_chibios_confs:-} ] ; then
    upgrade-chibios-confs
fi

pushd "$script_dir/qmk_firmware"
OIFS=$IFS
IFS=$'\n'
for file in $(find "$script_dir/qmk_firmware/keyboards" "$script_dir/qmk_firmware/platforms" -name 'chconf.h') ; do
    echo $file
    sed -i 's@#define CH_CFG_USE_JOBS                     TRUE@#define CH_CFG_USE_JOBS                     FALSE@g' "$file"
done
IFS=$OIFS
pcmd git commit -am "Disable ChibiOS jobs by default"
popd

pushd "$script_dir/qmk_firmware"
pcmd git push origin $target_branch --set-upstream --force-with-lease
popd
