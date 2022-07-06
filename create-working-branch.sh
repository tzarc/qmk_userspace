#!/bin/bash
# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later

set -eEuo pipefail

this_script="$(readlink -f "${BASH_SOURCE[0]}")"
script_dir="$(readlink -f "$(dirname "$this_script")")"

unset upgrade_chibios
unset upgrade_chibios_confs
#upgrade_chibios=1
#upgrade_chibios_confs=1

target_branch="generated-workarea"
target_qmk="develop"

target_chibios="svn-mirror/stable_21.11.x"
target_chibios_contrib="mirror/chibios-21.11.x"

if [ ! -z ${upgrade_chibios:-} ] ; then
    target_branch="generated-chibios-master-upgrade"
    target_qmk="develop"

    target_chibios="svn-mirror/stable_21.11.x"
    target_chibios_contrib="mirror/chibios-21.11.x"
fi

declare -a prs_to_apply
prs_to_apply+=(14877) # RP2040 support
prs_to_apply+=(16996) # Generic wear-leveling

declare -a cherry_picks
#cherry_picks+=(749aca03c90c9316189b58e3236bea9242f3990f) # RGB_MATRIX slave scan

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
    pcmd git clean -xfd
    pcmd git remote set-url origin git@github.com:tzarc/$repo_name.git
    pcmd git remote set-url origin git@github.com:tzarc/$repo_name.git --push
    pcmd git remote set-url upstream git@github.com:$repo_upstream/$repo_name.git \
        || pcmd git remote add upstream git@github.com:$repo_upstream/$repo_name.git
    pcmd git remote set-url upstream git@github.com:tzarc/$repo_name.git --push
    pcmd git remote set-url qmk git@github.com:qmk/$repo_name.git \
        || pcmd git remote add qmk git@github.com:qmk/$repo_name.git
    pcmd git remote set-url qmk git@github.com:tzarc/$repo_name.git --push
    pcmd git fetch qmk $repo_branch \
        || pcmd git fetch $repo_upstream $repo_branch \
        || pcmd git fetch origin $repo_branch
    pcmd git clean -xfd
    pcmd git checkout -f qmk/$repo_branch
    pcmd git branch -D $repo_branch || true
    pcmd git checkout -b $repo_branch
    pcmd git reset --hard qmk/$repo_branch \
        || pcmd git reset --hard $repo_upstream/$repo_branch \
        || pcmd git reset --hard origin/$repo_branch \
        || pcmd git reset --hard $repo_branch
    pcmd git push origin $repo_branch --force-with-lease
    pcmd git branch -D $target_branch || true
    pcmd git checkout -b $target_branch
    pcmd git reset --hard qmk/$repo_branch \
        || pcmd git reset --hard origin/$repo_branch \
        || pcmd git reset --hard $repo_branch
}

upgrade-chibios() {
    pushd "$script_dir/qmk_firmware"
    #./util/update_chibios_mirror.sh
    popd

    pushd "$script_dir/qmk_firmware/lib/chibios"
    hard_reset ChibiOS ChibiOS $target_chibios
    if [[ ! -z "$(git remote -v | grep origin | grep push | grep tzarc)" ]] ; then
        pcmd git push origin $target_branch --set-upstream --force
    else
        echo "Failed to push to tzarc/ChibiOS" >&2
        exit 1
    fi
    popd

    pushd "$script_dir/qmk_firmware/lib/chibios-contrib"
    hard_reset ChibiOS ChibiOS-Contrib $target_chibios_contrib
    if [[ ! -z "$(git remote -v | grep origin | grep push | grep tzarc)" ]] ; then
        pcmd git push origin $target_branch --set-upstream --force
    else
        echo "Failed to push to tzarc/ChibiOS-Contrib" >&2
        exit 1
    fi
    popd

    pushd "$script_dir/qmk_firmware"
    pcmd git add lib/chibios lib/chibios-contrib
    pcmd git commit -m 'Upgrade ChibiOS to master'
    popd
}

disable_chconf_extras() {
    for chconf in $(find "$script_dir/qmk_firmware/platforms" "$script_dir/qmk_firmware/keyboards" -name chconf.h) ; do
        cat "$chconf" \
            | sed \
                -e 's@#define CH_CFG_USE_OBJ_CACHES\(\s\+\)TRUE@#define CH_CFG_USE_OBJ_CACHES\1FALSE@g' \
                -e 's@#define CH_CFG_USE_DELEGATES\(\s\+\)TRUE@#define CH_CFG_USE_DELEGATES\1FALSE@g' \
                -e 's@#define CH_CFG_USE_JOBS\(\s\+\)TRUE@#define CH_CFG_USE_JOBS\1FALSE@g' \
                -e 's@#define CH_CFG_USE_FACTORY\(\s\+\)TRUE@#define CH_CFG_USE_FACTORY\1FALSE@g' \
                -e 's@#define CH_CFG_USE_MEMCORE\(\s\+\)FALSE@#define CH_CFG_USE_MEMCORE\1TRUE@g' \
            > "${chconf}.new"
        mv "${chconf}.new" "${chconf}"
    done
}

upgrade-chibios-confs() {
    pushd "$script_dir/qmk_firmware"

    OIFS=$IFS
    IFS=$'\n'
    for file in $(find "$script_dir/qmk_firmware/keyboards" "$script_dir/qmk_firmware/platforms" -name 'chconf.h') ; do
        echo $file

        sed -i 's@#define CH_CFG_USE_JOBS\s*TRUE@#define CH_CFG_USE_JOBS FALSE@g' "$file"
        sed -i 's@#define CH_CFG_USE_FACTORY\s*TRUE@#define CH_CFG_USE_FACTORY FALSE@g' "$file"
        sed -i 's@#define CH_CFG_USE_MEMCORE\s*FALSE@#define CH_CFG_USE_MEMCORE TRUE@g' "$file"

        if ! grep -q include_next "$file" ; then
            echo '#define CH_CFG_USE_OBJ_CACHES FALSE' >> "$file"
            echo '#define CH_CFG_USE_DELEGATES FALSE' >> "$file"
            echo '#define CH_CFG_USE_JOBS FALSE' >> "$file"
            echo '#define CH_CFG_USE_FACTORY FALSE' >> "$file"
            echo '#define CH_CFG_USE_MEMCORE TRUE' >> "$file"
        fi
    done
    IFS=$OIFS

    sed -i 's@revert_chibi_files "@#revert_chibi_files "@g' ./util/chibios_conf_updater.sh
    pcmd ./util/chibios_conf_updater.sh
    sed -i 's@#revert_chibi_files "@revert_chibi_files "@g' ./util/chibios_conf_updater.sh

    popd

    pushd "$script_dir"
    pcmd make clean
    popd

    disable_chconf_extras

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
upgrade-chibios
popd

pushd "$script_dir/qmk_firmware"
for pr in ${prs_to_apply[@]} ; do
    echo -e "\e[38;5;203mPR $pr\e[0m"
    set +e
    pcmd hub merge https://github.com/qmk/qmk_firmware/pull/${pr}
    if [[ $? != 0 ]] ; then
        echo "Error during merge, please fix and hit ENTER:"
        read DUMMY
    fi
    pcmd make git-submodule
    set -e
    pcmd git commit --amend -m "Merge qmk_firmware upstream PR ${pr}"
done
popd

pushd "$script_dir/qmk_firmware"
for hash in ${cherry_picks[@]} ; do
    echo -e "\e[38;5;203mCherry-picking $hash\e[0m"
    pcmd git cherry-pick $hash
    pcmd make git-submodule
done
popd

if [ ! -z ${upgrade_chibios_confs:-} ] ; then
    upgrade-chibios-confs
fi

pushd "$script_dir/qmk_firmware"
disable_chconf_extras
pcmd git commit -am "Fixup ChibiOS configs" || true
popd

pushd "$script_dir/qmk_firmware"
pcmd git push origin $target_branch --set-upstream --force-with-lease
popd
