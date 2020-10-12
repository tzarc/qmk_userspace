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

function_exists() {
    declare -f -F $1 > /dev/null
    return $?
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
    local builds=()
    local before_hash=()
    local after_hash=()
    local keyboard
    local chibios_board
    local ignore_checksum_mismatch
    local force_builds
    local no_mcuconf
    local keep_output_files

    while [[ ! -z "${1:-}" ]] ; do
        case "${1:-}" in
            --force)
                force_builds=1
                ;;
            --no-mcuconf)
                no_mcuconf=1
                ;;
            --keyboard)
                shift
                keyboard="$1"
                ;;
            --chibios-board)
                shift
                chibios_board="$1"
                ;;
            --ignore-checksum)
                shift
                ignore_checksum_mismatch="$1"
                ;;
            --keep-output)
                keep_output_files=1
                ;;
            *)
                builds+=($1)
                ;;
        esac
        shift
    done

    if [[ "${#builds[@]}" -eq 0 ]] ; then
        builds+=($keyboard)
    fi

    git reset --hard >/dev/null 2>&1
    git checkout -- . >/dev/null 2>&1

    if [[ ! -z "${force_builds:-}" ]] \
    || { [[ -f "keyboards/$keyboard/chconf.h" ]] && [[ -z "$(grep 'include_next' "keyboards/$keyboard/chconf.h" 2>/dev/null || true)" ]] ; } \
    || { [[ -f "keyboards/$keyboard/halconf.h" ]] && [[ -z "$(grep 'include_next' "keyboards/$keyboard/halconf.h" 2>/dev/null || true)" ]] ; } \
    || { [[ -f "keyboards/$keyboard/mcuconf.h" ]] && [[ -z "$(grep 'include_next' "keyboards/$keyboard/mcuconf.h" 2>/dev/null || true)" ]] ; } ; then
        echo "=========================================================================" | append_log
        echo "== $keyboard : $chibios_board" | append_log
        echo "=========================================================================" | append_log

        for build in ${builds[@]} ; do
            make distclean 2>&1 | append_log
            local binary_basename="$(make -j$(nproc) $build:default:dump_vars COMMAND_ENABLE=no SKIP_GIT=yes | grep -P '^TARGET=' | cut -d'=' -f2)"
            make -j$(nproc) $build:default:dump_vars COMMAND_ENABLE=no SKIP_GIT=yes 2>&1 > "$script_dir/chibios-upgrade-output/${binary_basename}_before_vars.txt"
            { make -j$(nproc) $build:default COMMAND_ENABLE=no SKIP_GIT=yes 2>&1 || true ; } | append_log
            cat ".build/obj_${binary_basename}/cflags.txt" | sed -e 's/ /\n/g' > "$script_dir/chibios-upgrade-output/${binary_basename}_before_cflags.txt"
            [[ ! -d "$script_dir/chibios-upgrade-output/${binary_basename}_before_build" ]] \
                || rm -rf "$script_dir/chibios-upgrade-output/${binary_basename}_before_build"
            mv ".build" "$script_dir/chibios-upgrade-output/${binary_basename}_before_build"
            find "$script_dir/chibios-upgrade-output/${binary_basename}_before_build" -name '*.lst' -exec sed -i 's#/tmp/cc......\.s#/tmp/ccTEMPFL.s#g' '{}' +
            arm-none-eabi-objdump -S "$script_dir/chibios-upgrade-output/${binary_basename}_before_build/${binary_basename}.elf" \
                > "$script_dir/chibios-upgrade-output/${binary_basename}_before_asm.txt"
            [[ -e "${binary_basename}.bin" ]] || exit 1
            local before=$(sha1sum "${binary_basename}.bin" | awk '{print $1}')
            before_hash+=($before)
        done

        if function_exists "${binary_basename}_beforeconf" ; then
            "${binary_basename}_beforeconf"
        fi

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

        if [[ -z "${no_mcuconf:-}" ]] && [[ -f "keyboards/$keyboard/mcuconf.h" ]] && [[ -z "$(grep 'include_next' "keyboards/$keyboard/mcuconf.h" 2>/dev/null || true)" ]] ; then
            echo "-------------------------------------------------------------------------" | append_log
            echo "-- $keyboard : migrating mcuconf.h" | append_log
            echo "-------------------------------------------------------------------------" | append_log
            ./bin/qmk chibios-confmigrate -o -d -i keyboards/$keyboard/mcuconf.h -r platforms/chibios/$chibios_board/configs/mcuconf.h 2>&1 | append_log
        fi

        # Fixup the 'BOARD = ...' line in rules.mk
        if [[ -e keyboards/$keyboard/rules.mk ]] ; then
            sed -i "s@^BOARD\s*=\s*.*@BOARD = $chibios_board@g" keyboards/$keyboard/rules.mk
        fi
        for build in ${builds[@]} ; do
            if [[ -e keyboards/$build/rules.mk ]] ; then
                sed -i "s@^BOARD\s*=\s*.*@BOARD = $chibios_board@g" keyboards/$build/rules.mk
            fi
        done

        if function_exists "${binary_basename}_afterconf" ; then
            "${binary_basename}_afterconf"
        fi

        for build in ${builds[@]} ; do
            make distclean 2>&1 | append_log
            local binary_basename="$(make -j$(nproc) $build:default:dump_vars COMMAND_ENABLE=no SKIP_GIT=yes | grep -P '^TARGET=' | cut -d'=' -f2)"
            make -j$(nproc) $build:default:dump_vars COMMAND_ENABLE=no SKIP_GIT=yes 2>&1 > "$script_dir/chibios-upgrade-output/${binary_basename}_after_vars.txt"
            { make -j$(nproc) $build:default COMMAND_ENABLE=no SKIP_GIT=yes 2>&1 || true ; } | append_log
            cat ".build/obj_${binary_basename}/cflags.txt" | sed -e 's/ /\n/g' > "$script_dir/chibios-upgrade-output/${binary_basename}_after_cflags.txt"
            [[ ! -d "$script_dir/chibios-upgrade-output/${binary_basename}_after_build" ]] \
                || rm -rf "$script_dir/chibios-upgrade-output/${binary_basename}_after_build"
            mv ".build" "$script_dir/chibios-upgrade-output/${binary_basename}_after_build"
            find "$script_dir/chibios-upgrade-output/${binary_basename}_after_build" -name '*.lst' -exec sed -i 's#/tmp/cc......\.s#/tmp/ccTEMPFL.s#g' '{}' +
            { arm-none-eabi-objdump -S "$script_dir/chibios-upgrade-output/${binary_basename}_after_build/${binary_basename}.elf" \
                > "$script_dir/chibios-upgrade-output/${binary_basename}_after_asm.txt" ; } || true
            [[ -e "${binary_basename}.bin" ]] || exit 1
            local after=$(sha1sum "${binary_basename}.bin" | awk '{print $1}' || echo "UNKNOWN")
            after_hash+=($after)
        done

        local commit_message="ChibiOS conf upgrade for $keyboard"$'\n'""$'\n'""
        if [[ ! -z "${ignore_checksum_mismatch:-}" ]] ; then
            commit_message="$commit_message"$'\n'"sha1sum mismatch ignored: ${ignore_checksum_mismatch}"$'\n'
        fi
        local builds_ok="yes"
        for (( i = 0 ; i < ${#builds[@]} ; i++ )) ; do
            unset before after
            local build=${builds[$i]}
            local before=${before_hash[$i]}
            local after=${after_hash[$i]}

            commit_message="$commit_message"$'\n'"- $build: $after"

            if [[ "$before" != "$after" ]] ; then
                hinfo "!!! $build:default \e[38;5;9msha1sum mismatch: $after != $before" 2>&1 | tee_log
                builds_ok="no"
            else
                reason=${ignore_checksum_mismatch:-matches}
                hinfo "!!! $build:default \e[38;5;10msha1sum $reason: $after" 2>&1 | tee_log
            fi
        done

        if [[ "$builds_ok" == "yes" ]] || [[ ! -z "${ignore_checksum_mismatch:-}" ]]; then
            for build in ${builds[@]} ; do
                if [[ -z "${keep_output_files:-}" ]] ; then
                    local binary_basename="$(make $build:default:dump_vars COMMAND_ENABLE=no SKIP_GIT=yes | grep -P '^TARGET=' | cut -d'=' -f2)"
                    rm -rf "$script_dir/chibios-upgrade-output/${binary_basename}_before_build" \
                            "$script_dir/chibios-upgrade-output/${binary_basename}_before_vars.txt" \
                            "$script_dir/chibios-upgrade-output/${binary_basename}_before_asm.txt" \
                            "$script_dir/chibios-upgrade-output/${binary_basename}_before_cflags.txt" \
                            "$script_dir/chibios-upgrade-output/${binary_basename}_after_build" \
                            "$script_dir/chibios-upgrade-output/${binary_basename}_after_vars.txt" \
                            "$script_dir/chibios-upgrade-output/${binary_basename}_after_asm.txt" \
                            "$script_dir/chibios-upgrade-output/${binary_basename}_after_cflags.txt"
                fi
            done

            git add -A
            git commit -qam "$commit_message"
        fi
    fi

    git reset --hard >/dev/null 2>&1
    git checkout -- . >/dev/null 2>&1
}

upgrade_base() {
    hard_reset qmk qmk_firmware develop
    pcmd make git-submodule

    # Migration scripts
    pcmd hub merge https://github.com/qmk/qmk_firmware/pull/9952
    pcmd git commit --amend -m "Merge qmk_firmware upstream PR 9952"
    # Fixup the ChibiOS conf upgrade script
    sed -i "s@ upstream/master@@g" util/chibios-upgrader.sh
    sed -i "s@ origin/master@@g" util/chibios-upgrader.sh
    pcmd git commit -am "Retarget ChibiOS upgrader script to current revision"
}

chavdai40_rev2_default_afterconf() {
    pushd "$script_dir/qmk_firmware"
    pcmd git rm -rf keyboards/chavdai40/boards
    popd
}

handwired_onekey_blackpill_f401_default_afterconf() {
    pushd "$script_dir/qmk_firmware"
    pcmd git rm platforms/chibios/BLACKPILL_STM32_F401/configs/chconf.h || true
    pcmd git rm platforms/chibios/BLACKPILL_STM32_F401/configs/halconf.h || true
    pcmd cp -f "$script_dir/chibios-upgrade-staging/handwired_onekey_blackpill_f401_halconf.h" \
        "$script_dir/qmk_firmware/keyboards/handwired/onekey/blackpill_f401/halconf.h"
    pcmd git add "$script_dir/qmk_firmware/keyboards/handwired/onekey/blackpill_f401/halconf.h"
    popd
}

handwired_onekey_blackpill_f411_default_afterconf() {
    pushd "$script_dir/qmk_firmware"
    pcmd git rm platforms/chibios/BLACKPILL_STM32_F411/configs/chconf.h || true
    pcmd git rm platforms/chibios/BLACKPILL_STM32_F411/configs/halconf.h || true
    pcmd cp -f "$script_dir/chibios-upgrade-staging/handwired_onekey_blackpill_f411_halconf.h" \
        "$script_dir/qmk_firmware/keyboards/handwired/onekey/blackpill_f411/halconf.h"
    pcmd git add "$script_dir/qmk_firmware/keyboards/handwired/onekey/blackpill_f411/halconf.h"
    popd
}

akegata_denki_device_one_default_afterconf() {
    pushd "$script_dir/qmk_firmware"
    pcmd git rm -rf keyboards/akegata_denki/device_one/boards
    popd
}

ergodox_stm32_default_afterconf() {
    pushd "$script_dir/qmk_firmware"
    pcmd git rm -rf keyboards/ergodox_stm32/boards
    popd
}

jm60_default_afterconf() {
    pushd "$script_dir/qmk_firmware"
    pcmd git rm -rf keyboards/jm60/boards
    popd
}

handwired_bluepill_bluepill70_default_afterconf() {
    pushd "$script_dir/qmk_firmware"
    pcmd git rm -rf keyboards/handwired/bluepill/boards

    echo >> keyboards/handwired/bluepill/bluepill70/config.h
    echo '// During the ChibiOS upgrade, we swapped base boards... replicate the original.' >> keyboards/handwired/bluepill/bluepill70/config.h
    echo '#define BOARD_GENERIC_STM32_F103' >> keyboards/handwired/bluepill/bluepill70/config.h

    echo >> keyboards/handwired/bluepill/bluepill70/rules.mk
    echo 'BOARD = STM32_F103_STM32DUINO' >> keyboards/handwired/bluepill/bluepill70/rules.mk
    popd
}

matrix_noah_default_afterconf() {
    pushd "$script_dir/qmk_firmware"
    pcmd git rm platforms/chibios/BLACKPILL_STM32_F411/configs/chconf.h || true
    pcmd git rm platforms/chibios/BLACKPILL_STM32_F411/configs/halconf.h || true
    pcmd git rm -rf keyboards/matrix/noah/boards
    pcmd cp -f "$script_dir/chibios-upgrade-staging/matrix_noah_board.h" \
        "$script_dir/qmk_firmware/keyboards/matrix/noah/board.h"
    pcmd git add "$script_dir/qmk_firmware/keyboards/matrix/noah/board.h"
}

matrix_m20add_default_afterconf() {
    pushd "$script_dir/qmk_firmware"
    pcmd git rm platforms/chibios/BLACKPILL_STM32_F411/configs/chconf.h || true
    pcmd git rm platforms/chibios/BLACKPILL_STM32_F411/configs/halconf.h || true
    pcmd git rm -rf keyboards/matrix/m20add/boards
    pcmd cp -f "$script_dir/chibios-upgrade-staging/matrix_m20add_board.h" \
        "$script_dir/qmk_firmware/keyboards/matrix/m20add/board.h"
    pcmd git add "$script_dir/qmk_firmware/keyboards/matrix/m20add/board.h"
}

upgrade_all_keyboards() {
    upgrade_one_keyboard --keyboard 1upkeyboards/sweet16/v2/proton_c --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard acheron/arctic --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard acheron/austin --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard acheron/keebspcb --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard acheron/shark --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard aeboards/ext65/rev2 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard ai03/vega --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard akegata_denki/device_one --chibios-board GENERIC_STM32_F042X6
    upgrade_one_keyboard --keyboard at_at/660m --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard boston_meetup/2019 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard cannonkeys/an_c --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard cannonkeys/atlas --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard cannonkeys/chimera65 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard cannonkeys/db60 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard cannonkeys/devastatingtkl --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard cannonkeys/instant60 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard cannonkeys/instant65 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard cannonkeys/iron165 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard cannonkeys/obliterated75 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard cannonkeys/ortho48 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard cannonkeys/ortho60 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard cannonkeys/ortho75 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard cannonkeys/practice60 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard cannonkeys/practice65 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard cannonkeys/rekt1800 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard cannonkeys/satisfaction75 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard cannonkeys/savage65 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard cannonkeys/tmov2 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard chavdai40 --chibios-board GENERIC_STM32_F042X6 chavdai40/rev1 chavdai40/rev2
    upgrade_one_keyboard --keyboard cheshire/curiosity --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard ckeys/thedora --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard clueboard/60 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard clueboard/66_hotswap/gen1 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard clueboard/66/rev4 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard clueboard/california --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard converter/siemens_tastatur --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard dztech/dz60rgb_ansi/v1 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard dztech/dz60rgb_wkl/v1 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard dztech/dz60rgb/v1 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard dztech/dz65rgb/v1 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard ergodox_infinity --chibios-board PJRC_TEENSY_3_1 --no-mcuconf
    upgrade_one_keyboard --keyboard ergodox_stm32 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard function96 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard generic_panda/panda65_01 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard hadron/ver3 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard handwired/bluepill/bluepill70 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard handwired/ck4x4 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard handwired/co60/rev6 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard handwired/co60/rev7 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard handwired/d48 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard handwired/ddg_56 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard handwired/floorboard --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard handwired/onekey/blackpill_f401 --chibios-board BLACKPILL_STM32_F401 --force
    upgrade_one_keyboard --keyboard handwired/onekey/blackpill_f411 --chibios-board BLACKPILL_STM32_F411 --force
    upgrade_one_keyboard --keyboard handwired/onekey/bluepill --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard handwired/onekey/proton_c --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard handwired/onekey/pytest --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard handwired/onekey/stm32f0_disco --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard handwired/onekey/teensy_32 --chibios-board PJRC_TEENSY_3_1 --no-mcuconf
    upgrade_one_keyboard --keyboard handwired/onekey/teensy_lc --chibios-board PJRC_TEENSY_LC --no-mcuconf
    upgrade_one_keyboard --keyboard handwired/riblee_f401 --chibios-board BLACKPILL_STM32_F401
    upgrade_one_keyboard --keyboard handwired/selene --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard handwired/steamvan/rev1 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard handwired/twadlee/tp69 --chibios-board PJRC_TEENSY_LC --no-mcuconf
    upgrade_one_keyboard --keyboard handwired/wulkan --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard handwired/z150 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard hs60/v2 --chibios-board GENERIC_STM32_F303XC hs60/v2/ansi hs60/v2/hhkb hs60/v2/iso
    upgrade_one_keyboard --keyboard infinity60 --chibios-board MCHCK_K20 --no-mcuconf
    upgrade_one_keyboard --keyboard jm60 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard k_type --chibios-board PJRC_TEENSY_3_1 --no-mcuconf
    upgrade_one_keyboard --keyboard kbdfans/kbd67/mkiirgb/v1 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard keebio/bdn9/rev2 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard keebio/choconum --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard kv/revt --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard matrix/m20add --chibios-board BLACKPILL_STM32_F411
    upgrade_one_keyboard --keyboard matrix/noah --chibios-board BLACKPILL_STM32_F411
    upgrade_one_keyboard --keyboard mechlovin/adelais --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard mechlovin/hannah60rgb --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard mechlovin/infinity87 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard moonlander --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard nebula12 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard nebula65 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard nemui --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard nk65 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard nk87 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard peiorisboards/ixora --chibios-board GENERIC_STM32_F042X6
    upgrade_one_keyboard --keyboard planck/ez --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard planck/rev6 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard preonic/rev3 --chibios-board GENERIC_STM32_F303XC
    upgrade_one_keyboard --keyboard projectkb/alice --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard projectkb/alice/rev1 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard projectkb/alice/rev2 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard projectkb/signature87 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard ramonimbao/squishy65 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard ramonimbao/wete --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard retro_75 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard satt/vision --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard tkc/candybar --chibios-board GENERIC_STM32_F072XB tkc/candybar/lefty tkc/candybar/righty
    upgrade_one_keyboard --keyboard tkc/candybar/lefty --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard tkc/candybar/righty --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard tkc/godspeed75 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard vinta --chibios-board GENERIC_STM32_F042X6
    upgrade_one_keyboard --keyboard westfoxtrot/prophet --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard whitefox --chibios-board PJRC_TEENSY_3_1 --no-mcuconf
    upgrade_one_keyboard --keyboard wolfmarkclub/wm1 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard xelus/trinityxttkl --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard xiaomi/mk02 --chibios-board GENERIC_STM32_F072XB --ignore-checksum "ignored as we've changed the base board from an F072 discovery to the generic F072 board in QMK"
    upgrade_one_keyboard --keyboard zvecr/split_blackpill --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard zvecr/zv48/f401 --chibios-board BLACKPILL_STM32_F401
    upgrade_one_keyboard --keyboard zvecr/zv48/f411 --chibios-board BLACKPILL_STM32_F411
}

leftover_chconf() {
    for file in $(find keyboards/ -name 'chconf.h') ; do
        if [[ -z "$(grep include_next "$file")" ]] ; then
            echo $file | sed -e 's#keyboards/##g' -e 's#/chconf.h##g'
        fi
    done | sort | uniq
}

leftover_halconf() {
    for file in $(find keyboards/ -name 'halconf.h') ; do
        if [[ -z "$(grep include_next "$file")" ]] ; then
            echo $file | sed -e 's#keyboards/##g' -e 's#/halconf.h##g'
        fi
    done | sort | uniq
}

leftover_mcuconf() {
    for file in $(find keyboards/ -name 'mcuconf.h') ; do
        if [[ -z "$(grep include_next "$file")" ]] ; then
            echo $file | sed -e 's#keyboards/##g' -e 's#/mcuconf.h##g'
        fi
    done | sort | uniq
}

leftover_boards() {
    { leftover_chconf ; leftover_halconf ; } | sort | uniq
}

print_leftovers() {
    leftovers=$(leftover_boards)

    echo -e "\e[1;35m$(echo "$leftovers" | wc -l) boards outstanding:\e[0m"
    for leftover in $leftovers ; do
        echo "$leftover: $(make $leftover:default:dump_vars | grep '^BOARD=')"
    done
}

pushd "$script_dir/qmk_firmware"

[[ -d "$script_dir/chibios-upgrade-output" ]] \
    || mkdir "$script_dir/chibios-upgrade-output"

echo -n > "$script_dir/chibios-upgrade-output/upgrade.log"

upgrade_base
upgrade_all_keyboards
git push --set-upstream origin $target_branch --force-with-lease
print_leftovers

popd
