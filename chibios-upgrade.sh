#!/bin/bash

set -eEuo pipefail

this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"

source_branch="develop"
upgrade_branch_name="chibios-upgrade-ver20.3.2"

chibios_tag="ver20.3.2"
contrib_branch="chibios-20.3.x"
target_tag="breaking_2021_q1"

pcmd() {
    echo -e "\e[38;5;203mExecuting:\e[38;5;131m $@\e[0m" >&2
    "$@"
}

setup_upgrade_branch() {
    pushd "$script_dir/qmk_firmware"
    pcmd git checkout "$source_branch"
    pcmd git fetch --all --tags --prune
    pcmd git branch -D "$upgrade_branch_name" || true
    pcmd git checkout -b "$upgrade_branch_name" "upstream/$source_branch"
    popd
}

upgrade_chibios() {
    #pushd "$script_dir/qmk_chibios"
    #pcmd git remote set-url origin --push git@github.com:qmk/ChibiOS.git
    #pcmd git remote set-url upstream --push git@github.com:qmk/ChibiOS.git
    #pcmd git tag -d "$target_tag" || true
    #pcmd git fetch --all --tags --prune
    #pcmd git tag -a -f -m "Adding tag $target_tag" "$target_tag" "tags/$chibios_tag"
    #pcmd git push origin "$target_tag" --force
    #popd

    pushd "$script_dir/qmk_firmware"
    pcmd git checkout "$upgrade_branch_name"
    pcmd git submodule update --init --recursive
    popd

    pushd "$script_dir/qmk_firmware/lib/chibios"
    pcmd git tag -d "$target_tag" || true
    pcmd git fetch --all --tags --prune
    pcmd git checkout "$target_tag"
    popd

    pushd "$script_dir/qmk_firmware"
    pcmd git checkout "$upgrade_branch_name"
    pcmd git commit -am "Update ChibiOS submodule"
    popd
}

upgrade_chibios_contrib() {
    #pushd "$script_dir/qmk_chibios-contrib"
    #pcmd git remote set-url origin --push git@github.com:qmk/ChibiOS-Contrib.git
    #pcmd git remote set-url upstream --push git@github.com:qmk/ChibiOS-Contrib.git
    #pcmd git tag -d "$target_tag" || true
    #pcmd git fetch --all --tags --prune
    #pcmd git checkout "$contrib_branch"
    #pcmd git reset --hard "upstream/$contrib_branch"
    #pcmd git tag -a -f -m "Adding tag $target_tag" "$target_tag"
    #pcmd git push origin "$target_tag" --force
    #popd

    pushd "$script_dir/qmk_firmware"
    pcmd git checkout "$upgrade_branch_name"
    pcmd git submodule update --init --recursive
    popd

    pushd "$script_dir/qmk_firmware/lib/chibios-contrib"
    pcmd git tag -d "$target_tag" || true
    pcmd git fetch --all --tags --prune
    pcmd git checkout "$target_tag"
    popd

    pushd "$script_dir/qmk_firmware"
    pcmd git checkout "$upgrade_branch_name"
    pcmd git commit -am "Update ChibiOS-Contrib submodule"
    popd
}

upgrade_configs() {
    pushd "$script_dir/qmk_firmware"
    ./util/chibios_conf_updater.sh

    for chconf in $(find "$script_dir/qmk_firmware/platforms" "$script_dir/qmk_firmware/keyboards" -name chconf.h) ; do
        cat "$chconf" \
            | sed \
                -e 's@#define CH_CFG_USE_OBJ_CACHES               TRUE@#define CH_CFG_USE_OBJ_CACHES               FALSE@g' \
                -e 's@#define CH_CFG_USE_DELEGATES                TRUE@#define CH_CFG_USE_DELEGATES                FALSE@g' \
                -e 's@#define CH_CFG_USE_JOBS                     TRUE@#define CH_CFG_USE_JOBS                     FALSE@g' \
            > "${chconf}.new"
        mv "${chconf}.new" "${chconf}"
    done

    pcmd git commit -am "Update ChibiOS config files"

    pcmd git push --set-upstream origin "$upgrade_branch_name" --force
    popd
}

setup_upgrade_branch
upgrade_chibios
upgrade_chibios_contrib
upgrade_configs
