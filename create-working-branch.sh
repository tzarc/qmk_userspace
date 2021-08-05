#!/bin/bash

set -eEuo pipefail

this_script="$(readlink -f "${BASH_SOURCE[0]}")"
script_dir="$(readlink -f "$(dirname "$this_script")")"

unset upgrade_chibios
unset upgrade_chibios_confs
#upgrade_chibios=1
#upgrade_chibios_confs=1

target_branch="generated-workarea"
target_qmk="develop"
target_chibios="svn-mirror/stable_20.3.x"
target_chibios_contrib="chibios-20.3.x"
if [ ! -z ${upgrade_chibios:-} ] ; then
target_branch="generated-chibios-master-upgrade"
target_qmk="develop"
target_chibios="svn-mirror/trunk"
target_chibios_contrib="chibios-20.3.x"
fi

declare -a prs_to_apply
prs_to_apply+=(10174) # Quantum Painter
prs_to_apply+=(13286) # encoder mapping
prs_to_apply+=(13523) # Split one-hand
prs_to_apply+=(13896) # ChibiOS-Contrib update

declare -a cherry_picks
#cherry_picks+=(749aca03c90c9316189b58e3236bea9242f3990f) # RGB_MATRIX slave scan

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
    pcmd git clean -xfd
    pcmd git remote set-url origin git@github.com:tzarc/$repo_name.git
    pcmd git remote set-url origin git@github.com:tzarc/$repo_name.git --push
    pcmd git remote set-url upstream git@github.com:$repo_upstream/$repo_name.git
    pcmd git remote set-url upstream git@github.com:$repo_upstream/$repo_name.git --push
    pcmd git fetch --all --tags --prune
    pcmd git fetch --unshallow upstream || true
    pcmd git clean -xfd
    pcmd git checkout -f $repo_branch
    pcmd git reset --hard upstream/$repo_branch || pcmd git reset --hard origin/$repo_branch
    pcmd git push origin $repo_branch --force-with-lease
    pcmd git branch -D $target_branch || true
    pcmd git checkout -b $target_branch
    pcmd git reset --hard upstream/$repo_branch || pcmd git reset --hard origin/$repo_branch
}

upgrade-chibios() {
    pushd "$script_dir/qmk_firmware/lib/chibios"
    git tag -d ver20.3.1
    hard_reset ChibiOS ChibiOS $target_chibios
    pcmd git push origin $target_branch --set-upstream --force-with-lease
    popd

    pushd "$script_dir/qmk_firmware/lib/chibios-contrib"
    hard_reset ChibiOS ChibiOS-Contrib $target_chibios_contrib
    pcmd git push origin $target_branch --set-upstream --force-with-lease
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

exit 0

# Set up the Djinn branch
pushd "$script_dir/qmk_firmware"
pcmd git branch -D djinn || true
pcmd git checkout -b djinn "$target_branch"
pcmd git reset --hard "$target_branch"
pcmd make git-submodule
[[ -d keyboards/tzarc ]] || mkdir -p keyboards/tzarc
pcmd rsync -avvP "$script_dir/tzarc-djinn/"* keyboards/tzarc/djinn
pcmd rm -rf keyboards/tzarc/djinn/keymaps/tzarc
pcmd git add keyboards/tzarc/djinn
pcmd git commit -m "Import Djinn code."
pcmd git push origin djinn --set-upstream --force-with-lease
pcmd git st
popd

pushd "$script_dir/qmk_firmware"
pcmd git checkout "$target_branch"
popd
