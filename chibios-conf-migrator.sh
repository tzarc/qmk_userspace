#!/bin/bash

umask 022

set -eEuo pipefail

target_branch="generated-chibios-conf-migration"
this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_firmware_dir="$(realpath "$script_dir/qmk_firmware/")" # change this once moved to util

cd "$qmk_firmware_dir"

spacer() {
    for f in $(seq 1 10) ; do
        echo
    done
}

hinfo() {
    echo -e "\e[38;5;14m$@\e[0m"
}

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
    pcmd git fetch upstream || true
    pcmd git checkout -f $repo_branch
    pcmd git reset --hard upstream/$repo_branch
    pcmd git push origin $repo_branch --force-with-lease
    pcmd git branch -D $target_branch || true
    pcmd git checkout -b $target_branch || true
    pcmd git reset --hard upstream/$repo_branch
}

append_log() {
    cat - | sed -r 's/\x1B\[[0-9;]*[JKmsu]//g' >> "$script_dir/chibios-upgrade-output/upgrade.log"
}

tee_log() {
    local output=$(cat -)
    echo -e "$output"
    echo -e "$output" | append_log
}

upgrade_one_keyboard() {
    local keyboard="$1"
    local chibios_board="$2"

    git checkout -- .

    if [[ -f "keyboards/$keyboard/chconf.h" ]] && [[ -z "$(grep 'include_next' "keyboards/$keyboard/chconf.h" || true)" ]] \
    || [[ -f "keyboards/$keyboard/halconf.h" ]] && [[ -z "$(grep 'include_next' "keyboards/$keyboard/halconf.h" || true)" ]] \
    || [[ -f "keyboards/$keyboard/mcuconf.h" ]] && [[ -z "$(grep 'include_next' "keyboards/$keyboard/mcuconf.h" || true)" ]] ; then
        echo "=========================================================================" | append_log
        echo "== $keyboard : $chibios_board" | append_log
        echo "=========================================================================" | append_log

        make distclean 2>&1 | append_log
        local binary_basename="$(make $keyboard:default:dump_vars COMMAND_ENABLE=no SKIP_GIT=yes | grep -P '^TARGET=' | cut -d'=' -f2)"
        make $keyboard:default:dump_vars COMMAND_ENABLE=no SKIP_GIT=yes 2>&1 > "$script_dir/chibios-upgrade-output/${binary_basename}_before_vars.txt"
        { make -j -O $keyboard:default COMMAND_ENABLE=no SKIP_GIT=yes 2>&1 || true ; } | append_log
        cat ".build/obj_${binary_basename}/cflags.txt" | sed -e 's/ /\n/g' > "$script_dir/chibios-upgrade-output/${binary_basename}_before_cflags.txt"
        [[ ! -d "$script_dir/chibios-upgrade-output/${binary_basename}_before_build" ]] \
            || rm -rf "$script_dir/chibios-upgrade-output/${binary_basename}_before_build"
        mv ".build" "$script_dir/chibios-upgrade-output/${binary_basename}_before_build"
        find "$script_dir/chibios-upgrade-output/${binary_basename}_before_build" -name '*.lst' -exec sed -i 's#/tmp/cc......\.s#/tmp/ccTEMPFL.s#g' '{}' +
        arm-none-eabi-objdump -S "$script_dir/chibios-upgrade-output/${binary_basename}_before_build/${binary_basename}.elf" \
            > "$script_dir/chibios-upgrade-output/${binary_basename}_before_asm.txt"
        local before=$(sha1sum "${binary_basename}.bin" | awk '{print $1}')

        if [[ -f "keyboards/$keyboard/chconf.h" ]] && [[ -z "$(grep 'include_next' "keyboards/$keyboard/chconf.h" 2>/dev/null || true)" ]] ; then
            echo "-------------------------------------------------------------------------" | append_log
            echo "-- $keyboard : migrating chconf.h" | append_log
            echo "-------------------------------------------------------------------------" | append_log
            ./bin/qmk chibios-confmigrate -o -d -i keyboards/$keyboard/chconf.h -r platforms/chibios/common/configs/chconf.h 2>&1 | append_log
        fi

        if [[ -f "keyboards/$keyboard/halconf.h" ]] && [[ -z "$(grep 'include_next' "keyboards/$keyboard/halconf.h" 2>/dev/null || true)" ]] ; then
            echo "-------------------------------------------------------------------------" | append_log
            echo "-- $keyboard : migrating halconf.h" | append_log
            echo "-------------------------------------------------------------------------" | append_log
            ./bin/qmk chibios-confmigrate -o -d -i keyboards/$keyboard/halconf.h -r platforms/chibios/common/configs/halconf.h 2>&1 | append_log
        fi

        if [[ -f "keyboards/$keyboard/mcuconf.h" ]] && [[ -z "$(grep 'include_next' "keyboards/$keyboard/mcuconf.h" 2>/dev/null || true)" ]] ; then
            echo "-------------------------------------------------------------------------" | append_log
            echo "-- $keyboard : migrating mcuconf.h" | append_log
            echo "-------------------------------------------------------------------------" | append_log
            ./bin/qmk chibios-confmigrate -o -d -i keyboards/$keyboard/mcuconf.h -r platforms/chibios/$chibios_board/configs/mcuconf.h 2>&1 | append_log
        fi

        make distclean 2>&1 | append_log
        make $keyboard:default:dump_vars COMMAND_ENABLE=no SKIP_GIT=yes 2>&1 > "$script_dir/chibios-upgrade-output/${binary_basename}_after_vars.txt"
        { make -j -O $keyboard:default COMMAND_ENABLE=no SKIP_GIT=yes 2>&1 || true ; } | append_log
        cat ".build/obj_${binary_basename}/cflags.txt" | sed -e 's/ /\n/g' > "$script_dir/chibios-upgrade-output/${binary_basename}_after_cflags.txt"
        [[ ! -d "$script_dir/chibios-upgrade-output/${binary_basename}_after_build" ]] \
            || rm -rf "$script_dir/chibios-upgrade-output/${binary_basename}_after_build"
        mv ".build" "$script_dir/chibios-upgrade-output/${binary_basename}_after_build"
        find "$script_dir/chibios-upgrade-output/${binary_basename}_after_build" -name '*.lst' -exec sed -i 's#/tmp/cc......\.s#/tmp/ccTEMPFL.s#g' '{}' +
        { arm-none-eabi-objdump -S "$script_dir/chibios-upgrade-output/${binary_basename}_after_build/${binary_basename}.elf" \
            > "$script_dir/chibios-upgrade-output/${binary_basename}_after_asm.txt" ; } || true
        local after=$(sha1sum "${binary_basename}.bin" | awk '{print $1}' || echo "UNKNOWN")

        if [[ "$before" != "$after" ]] ; then
            hinfo "!!! $keyboard:default \e[38;5;9msha1sum mismatch: $after != $before" 2>&1 | tee_log
        else
            hinfo "!!! $keyboard:default \e[38;5;10msha1sum matches: $after" 2>&1 | tee_log
            git add -A
            git commit -qam "$(printf "ChibiOS conf upgrade for $keyboard\n\n- Matching sha1sum between both builds: $after")"
            rm -rf "$script_dir/chibios-upgrade-output/${binary_basename}_before_build" \
                   "$script_dir/chibios-upgrade-output/${binary_basename}_before_vars.txt" \
                   "$script_dir/chibios-upgrade-output/${binary_basename}_before_asm.txt" \
                   "$script_dir/chibios-upgrade-output/${binary_basename}_before_cflags.txt" \
                   "$script_dir/chibios-upgrade-output/${binary_basename}_after_build" \
                   "$script_dir/chibios-upgrade-output/${binary_basename}_after_vars.txt" \
                   "$script_dir/chibios-upgrade-output/${binary_basename}_after_asm.txt" \
                   "$script_dir/chibios-upgrade-output/${binary_basename}_after_cflags.txt"
        fi
    fi

    git checkout -- .
}

pushd "$script_dir/qmk_firmware"

[[ -d "$script_dir/chibios-upgrade-output" ]] \
    || mkdir "$script_dir/chibios-upgrade-output"

echo -n > "$script_dir/chibios-upgrade-output/upgrade.log"

if [[ 1 == 1 ]] ; then
    hard_reset qmk qmk_firmware develop
    pcmd make git-submodule

    # Migration scripts
    pcmd hub merge https://github.com/qmk/qmk_firmware/pull/9952
    pcmd git commit --amend -m "Merge qmk_firmware upstream PR 9952"
    # Early init conversions
    pcmd hub merge https://github.com/qmk/qmk_firmware/pull/10214
    pcmd git commit --amend -m "Merge qmk_firmware upstream PR 10214"

    pcmd git checkout upstream/develop -- keyboards/cannonkeys/an_c/mcuconf.h
    pcmd cp keyboards/cannonkeys/an_c/mcuconf.h platforms/chibios/GENERIC_STM32_F072XB/configs/mcuconf.h
    pcmd git add -A
    pcmd git commit -m "Copy F072 mcuconf.h"
fi

upgrade_one_keyboard 1upkeyboards/sweet16/v2/proton_c GENERIC_STM32_F303XC
upgrade_one_keyboard acheron/arctic GENERIC_STM32_F072XB
upgrade_one_keyboard acheron/austin GENERIC_STM32_F072XB
upgrade_one_keyboard acheron/keebspcb GENERIC_STM32_F072XB
upgrade_one_keyboard acheron/shark GENERIC_STM32_F303XC
upgrade_one_keyboard aeboards/ext65/rev2 GENERIC_STM32_F072XB
upgrade_one_keyboard ai03/vega GENERIC_STM32_F072XB
upgrade_one_keyboard at_at/660m GENERIC_STM32_F072XB
upgrade_one_keyboard boston_meetup/2019 GENERIC_STM32_F303XC
upgrade_one_keyboard cannonkeys/an_c GENERIC_STM32_F072XB
upgrade_one_keyboard cannonkeys/atlas GENERIC_STM32_F072XB
upgrade_one_keyboard cannonkeys/chimera65 GENERIC_STM32_F072XB
upgrade_one_keyboard cannonkeys/db60 GENERIC_STM32_F072XB
upgrade_one_keyboard cannonkeys/devastatingtkl GENERIC_STM32_F072XB
upgrade_one_keyboard cannonkeys/instant60 GENERIC_STM32_F072XB
upgrade_one_keyboard cannonkeys/instant65 GENERIC_STM32_F072XB
upgrade_one_keyboard cannonkeys/iron165 GENERIC_STM32_F072XB
upgrade_one_keyboard cannonkeys/obliterated75 GENERIC_STM32_F072XB
upgrade_one_keyboard cannonkeys/rekt1800 GENERIC_STM32_F072XB
upgrade_one_keyboard cannonkeys/satisfaction75 GENERIC_STM32_F072XB
upgrade_one_keyboard cannonkeys/savage65 GENERIC_STM32_F072XB
upgrade_one_keyboard cannonkeys/tmov2 GENERIC_STM32_F072XB
upgrade_one_keyboard cheshire/curiosity GENERIC_STM32_F072XB
upgrade_one_keyboard ckeys/thedora GENERIC_STM32_F303XC
upgrade_one_keyboard clueboard/60 GENERIC_STM32_F303XC
upgrade_one_keyboard clueboard/66_hotswap/gen1 GENERIC_STM32_F303XC
upgrade_one_keyboard clueboard/66/rev4 GENERIC_STM32_F303XC
upgrade_one_keyboard clueboard/california GENERIC_STM32_F303XC
upgrade_one_keyboard dztech/dz60rgb_ansi/v1 GENERIC_STM32_F303XC
upgrade_one_keyboard dztech/dz60rgb_wkl/v1 GENERIC_STM32_F303XC
upgrade_one_keyboard dztech/dz60rgb/v1 GENERIC_STM32_F303XC
upgrade_one_keyboard dztech/dz65rgb/v1 GENERIC_STM32_F303XC
upgrade_one_keyboard function96 GENERIC_STM32_F072XB
upgrade_one_keyboard generic_panda/panda65_01 GENERIC_STM32_F072XB
upgrade_one_keyboard hadron/ver3 GENERIC_STM32_F303XC
upgrade_one_keyboard handwired/ck4x4 GENERIC_STM32_F072XB
upgrade_one_keyboard handwired/co60/rev6 GENERIC_STM32_F303XC
upgrade_one_keyboard handwired/co60/rev7 GENERIC_STM32_F303XC
upgrade_one_keyboard handwired/d48 GENERIC_STM32_F303XC
upgrade_one_keyboard handwired/ddg_56 GENERIC_STM32_F303XC
upgrade_one_keyboard handwired/floorboard GENERIC_STM32_F303XC
upgrade_one_keyboard handwired/onekey/proton_c GENERIC_STM32_F303XC
upgrade_one_keyboard handwired/onekey/pytest GENERIC_STM32_F303XC
upgrade_one_keyboard handwired/onekey/stm32f0_disco GENERIC_STM32_F072XB
upgrade_one_keyboard handwired/selene GENERIC_STM32_F303XC
upgrade_one_keyboard handwired/steamvan/rev1 GENERIC_STM32_F303XC
upgrade_one_keyboard handwired/wulkan GENERIC_STM32_F303XC
upgrade_one_keyboard hs60/v2/ansi GENERIC_STM32_F303XC
upgrade_one_keyboard hs60/v2/hhkb GENERIC_STM32_F303XC
upgrade_one_keyboard hs60/v2/iso GENERIC_STM32_F303XC
upgrade_one_keyboard kbdfans/kbd67/mkiirgb/v1 GENERIC_STM32_F303XC
upgrade_one_keyboard keebio/bdn9/rev2 GENERIC_STM32_F072XB
upgrade_one_keyboard keebio/choconum GENERIC_STM32_F072XB
upgrade_one_keyboard kv/revt GENERIC_STM32_F303XC
upgrade_one_keyboard mechlovin/adelais GENERIC_STM32_F303XC
upgrade_one_keyboard mechlovin/hannah60rgb GENERIC_STM32_F303XC
upgrade_one_keyboard mechlovin/infinity87 GENERIC_STM32_F303XC
upgrade_one_keyboard moonlander GENERIC_STM32_F303XC
upgrade_one_keyboard nebula12 GENERIC_STM32_F072XB
upgrade_one_keyboard nebula65 GENERIC_STM32_F303XC
upgrade_one_keyboard nemui GENERIC_STM32_F072XB
upgrade_one_keyboard nk65 GENERIC_STM32_F303XC
upgrade_one_keyboard nk87 GENERIC_STM32_F303XC
upgrade_one_keyboard planck/ez GENERIC_STM32_F303XC
upgrade_one_keyboard planck/rev6 GENERIC_STM32_F303XC
upgrade_one_keyboard preonic/rev3 GENERIC_STM32_F303XC
upgrade_one_keyboard projectkb/alice/rev1 GENERIC_STM32_F072XB
upgrade_one_keyboard projectkb/alice/rev2 GENERIC_STM32_F072XB
upgrade_one_keyboard projectkb/signature87 GENERIC_STM32_F072XB
upgrade_one_keyboard ramonimbao/wete GENERIC_STM32_F072XB
upgrade_one_keyboard retro_75 GENERIC_STM32_F072XB
upgrade_one_keyboard satt/vision GENERIC_STM32_F072XB
upgrade_one_keyboard tkc/candybar/lefty GENERIC_STM32_F072XB
upgrade_one_keyboard tkc/candybar/righty GENERIC_STM32_F072XB
upgrade_one_keyboard tkc/godspeed75 GENERIC_STM32_F072XB
upgrade_one_keyboard westfoxtrot/prophet GENERIC_STM32_F072XB
upgrade_one_keyboard xelus/trinityxttkl GENERIC_STM32_F072XB
upgrade_one_keyboard xiaomi/mk02 GENERIC_STM32_F072XB

git push --set-upstream origin $target_branch --force-with-lease

leftover_boards() {
    for file in $(find keyboards/ -name 'chconf.h' -or -name 'halconf.h' -or -name 'mcuconf.h') ; do
        if [[ -z "$(grep include_next "$file")" ]] ; then
            echo $file | sed -e 's#keyboards/##g' -e 's#/chconf.h##g' -e 's#/halconf.h##g' -e 's#/mcuconf.h##g'
        fi
    done | sort | uniq
}

leftovers=$(leftover_boards)
echo "$leftovers"
echo -e "\e[1;35m$(echo "$leftovers" | wc -l) boards outstanding\e[0m"

popd
