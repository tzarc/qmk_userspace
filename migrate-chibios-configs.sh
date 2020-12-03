#!/bin/bash

umask 022

set -eEuo pipefail
this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"
qmk_firmware_dir="$(realpath "$script_dir/qmk_firmware/")" # change this once moved to util
validation_output="$script_dir/validation-output"

source_branch="chibios-defaults-off-take2"
branch_under_test="generated-chibios-conf-migration"

export PATH=/home/nickb/gcc-arm/gcc-arm-none-eabi-8-2018-q4-major/bin:$PATH

[[ -d "$validation_output" ]] || mkdir -p "$validation_output"
echo -n >"$validation_output/upgrade.log"

fail_build() {
    touch "$validation_output/.failed"
}

is_failed() {
    if [[ -f "$validation_output/.failed" ]] ; then
        return 0
    fi
    return 1
}

clear_failure() {
    if [[ -f "$validation_output/.failed" ]] ; then
        rm -f "$validation_output/.failed"
    fi
}

pcmd() {
    echo -e "\e[38;5;203mExecuting:\e[38;5;131m $@\e[0m" >&2
    "$@"
}

append_log() {
    cat - | sed -r 's/\x1B\[[0-9;]*[JKmsu]//g' >> "$validation_output/upgrade.log"
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
    pcmd git reset --hard upstream/$repo_branch || pcmd git reset --hard $repo_branch
    pcmd git push origin $repo_branch --force-with-lease
}

build_single() {
    local build_target=$1
    local build_stage=$2
    local extraflags=${3:-}

    make distclean 2>&1 | append_log
    local binary_basename="$(make ${build_target}:default:dump_vars COMMAND_ENABLE=no SKIP_VERSION=yes SKIP_GIT=yes ${extraflags:-} | grep -P '^TARGET=' | cut -d'=' -f2)"
    make ${build_target}:default:dump_vars COMMAND_ENABLE=no SKIP_VERSION=yes SKIP_GIT=yes ${extraflags:-} 2>&1 > "${validation_output}/${binary_basename}_${build_stage}_vars.txt"
    { make -j$(nproc) ${build_target}:default COMMAND_ENABLE=no SKIP_VERSION=yes SKIP_GIT=yes ${extraflags:-} 2>&1 || true ; } | append_log
    cat ".build/obj_${binary_basename}/cflags.txt" | sed -e 's/ /\n/g' > "$validation_output/${binary_basename}_${build_stage}_cflags.txt"
    [[ ! -d "$validation_output/${binary_basename}_${build_stage}_build" ]] \
        || rm -rf "$validation_output/${binary_basename}_${build_stage}_build"
    mv ".build" "$validation_output/${binary_basename}_${build_stage}_build"
    find "$validation_output/${binary_basename}_${build_stage}_build" -name '*.lst' -exec sed -i 's#/tmp/cc......\.s#/tmp/ccTEMPFL.s#g' '{}' +
    arm-none-eabi-objdump -S "$validation_output/${binary_basename}_${build_stage}_build/${binary_basename}.elf" \
        > "$validation_output/${binary_basename}_${build_stage}_asm.txt"
    [[ -e "${binary_basename}.bin" ]] || { fail_build ; return 1 ; }
    sha1sum "${binary_basename}.bin" | awk '{print $1}'
}

validate_build() {
    local build_target=$1
    local extraflags=${2:-}

    clear_failure
    pushd "$qmk_firmware_dir" >/dev/null 2>&1

    git clean -xfd >/dev/null 2>&1
    git checkout -- . >/dev/null 2>&1
    git checkout $source_branch >/dev/null 2>&1
    local before="$(build_single "$build_target" before ${extraflags:-})"

    git clean -xfd >/dev/null 2>&1
    git checkout -- . >/dev/null 2>&1
    git checkout $branch_under_test >/dev/null 2>&1
    local after="$(build_single "$build_target" after ${extraflags:-})"

    if [[ "$before" == "$after" ]] ; then
        printf '\e[1;32m%50s - %s\e[0m\n' "$build_target" "$before"
        local binary_basename="$(make ${build_target}:default:dump_vars COMMAND_ENABLE=no SKIP_VERSION=yes SKIP_GIT=yes ${extraflags:-} | grep -P '^TARGET=' | cut -d'=' -f2)"
        rm -rf "$validation_output/"${binary_basename}*
    else
        printf '\e[1;31m%50s - %s != %s\e[0m\n' "$build_target" "$before" "$after"
        fail_build
    fi

    popd >/dev/null 2>&1
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

    pushd "$qmk_firmware_dir" >/dev/null 2>&1
    git checkout $branch_under_test >/dev/null 2>&1
    git clean -xfd >/dev/null 2>&1
    git reset --hard >/dev/null 2>&1
    git checkout -- . >/dev/null 2>&1

    if [[ -f "keyboards/$keyboard/chconf.h" ]] && [[ -z "$(grep 'include_next' "keyboards/$keyboard/chconf.h" 2>/dev/null || true)" ]] ; then
        echo "-------------------------------------------------------------------------" | append_log
        echo "-- $keyboard : migrating chconf.h" | append_log
        echo "-------------------------------------------------------------------------" | append_log
        local chconf_path="platforms/chibios/common/configs/chconf.h"
        [[ ! -f "platforms/chibios/$chibios_board/configs/chconf.h" ]] || chconf_path="platforms/chibios/$chibios_board/configs/chconf.h"
        ./bin/qmk chibios-confmigrate -o -d -i keyboards/$keyboard/chconf.h -r $chconf_path 2>&1 | append_log
    fi

    if [[ -f "keyboards/$keyboard/halconf.h" ]] && [[ -z "$(grep 'include_next' "keyboards/$keyboard/halconf.h" 2>/dev/null || true)" ]] ; then
        echo "-------------------------------------------------------------------------" | append_log
        echo "-- $keyboard : migrating halconf.h" | append_log
        echo "-------------------------------------------------------------------------" | append_log
        local halconf_path="platforms/chibios/common/configs/halconf.h"
        [[ ! -f "platforms/chibios/$chibios_board/configs/halconf.h" ]] || halconf_path="platforms/chibios/$chibios_board/configs/halconf.h"
        ./bin/qmk chibios-confmigrate -o -d -i keyboards/$keyboard/halconf.h -r $halconf_path 2>&1 | append_log
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

    local commit_message="ChibiOS conf upgrade for $keyboard"$'\n'""
    git add -A
    if git commit -qm "$commit_message" >/dev/null 2>&1 ; then
        for build in ${builds[@]} ; do
            output=$(validate_build $build)
            echo -e "$output"
            commit_message="${commit_message}"$'\n'"$(echo -e ${output} | sed -r "s/\x1B\[([0-9]{1,3}(;[0-9]{1,2})?)?[mGK]//g")"
        done

        if is_failed ; then
            clear_failure
            git reset --hard ${branch_under_test}^
        else
            git commit --amend -qm "$commit_message" || true
        fi
    fi

    popd >/dev/null 2>&1
}

preconfigure_branch() {
    pushd "$qmk_firmware_dir" >/dev/null 2>&1
    hard_reset qmk qmk_firmware $source_branch
    pcmd make git-submodule
    pcmd git branch -D $branch_under_test || true
    pcmd git checkout -b $branch_under_test
    popd >/dev/null 2>&1
}

upgrade_all_keyboards()  {
    upgrade_one_keyboard --keyboard k_type --chibios-board IC_TEENSY_3_1 --no-mcuconf
    upgrade_one_keyboard --keyboard whitefox --chibios-board IC_TEENSY_3_1 --no-mcuconf
    upgrade_one_keyboard --keyboard acheron/arctic --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard acheron/austin --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard acheron/keebspcb --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard acheron/shark --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard aeboards/ext65/rev2 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard ai03/vega --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard at_at/660m --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard boston_meetup/2019 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard bt66tech/bt66tech60 --chibios-board STM32_F103_STM32DUINO
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
    upgrade_one_keyboard --keyboard cheshire/curiosity --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard ckeys/thedora --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard clueboard/60 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard clueboard/66_hotswap/gen1 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard clueboard/66/rev4 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard clueboard/california --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard coarse/cordillera --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard converter/siemens_tastatur --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard dekunukem/duckypad --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard dztech/dz60rgb_ansi/v1 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard dztech/dz60rgb_wkl/v1 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard dztech/dz60rgb/v1 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard dztech/dz65rgb/v1 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard ergodox_infinity --chibios-board PJRC_TEENSY_3_1 --no-mcuconf
    upgrade_one_keyboard --keyboard function96 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard generic_panda/panda65_01 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard hadron/ver3 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard handwired/ck4x4 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard handwired/co60/rev6 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard handwired/co60/rev7 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard handwired/d48 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard handwired/ddg_56 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard handwired/floorboard --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard handwired/onekey/blackpill_f401 --chibios-board BLACKPILL_STM32_F401
    upgrade_one_keyboard --keyboard handwired/onekey/blackpill_f411 --chibios-board BLACKPILL_STM32_F411
    upgrade_one_keyboard --keyboard handwired/onekey/bluepill --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard handwired/onekey/proton_c --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard handwired/onekey/pytest --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard handwired/onekey/stm32f0_disco --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard handwired/onekey/teensy_32 --chibios-board PJRC_TEENSY_3_1 --no-mcuconf
    upgrade_one_keyboard --keyboard handwired/onekey/teensy_lc --chibios-board PJRC_TEENSY_LC --no-mcuconf
    upgrade_one_keyboard --keyboard handwired/pill60/blackpill_f401 --chibios-board BLACKPILL_STM32_F401
    upgrade_one_keyboard --keyboard handwired/pill60/blackpill_f411 --chibios-board BLACKPILL_STM32_F411
    upgrade_one_keyboard --keyboard handwired/pill60/bluepill --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard handwired/riblee_f401 --chibios-board BLACKPILL_STM32_F401
    upgrade_one_keyboard --keyboard handwired/riblee_f411 --chibios-board BLACKPILL_STM32_F411
    upgrade_one_keyboard --keyboard handwired/selene --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard handwired/steamvan/rev1 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard handwired/t111 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard handwired/twadlee/tp69 --chibios-board PJRC_TEENSY_LC --no-mcuconf
    upgrade_one_keyboard --keyboard handwired/wulkan --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard handwired/z150 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard hs60/v2 --chibios-board QMK_PROTON_C hs60/v2/ansi hs60/v2/hhkb hs60/v2/iso
    upgrade_one_keyboard --keyboard infinity60 --chibios-board MCHCK_K20 --no-mcuconf
    upgrade_one_keyboard --keyboard kbdfans/kbd67/mkiirgb/v1 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard keebio/bdn9/rev2 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard keebio/choconum --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard kv/revt --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard mechlovin/adelais --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard mechlovin/hannah60rgb --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard mechlovin/infinity87 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard misterknife/knife66 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard moonlander --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard nebula12 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard nebula68 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard nemui --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard nk65 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard nk87 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard peiorisboards/ixora --chibios-board GENERIC_STM32_F042X6
    upgrade_one_keyboard --keyboard phoenix --chibios-board BLACKPILL_STM32_F401
    upgrade_one_keyboard --keyboard planck/ez --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard planck/rev6 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard polilla --chibios-board GENERIC_STM32_F042X6
    upgrade_one_keyboard --keyboard preonic/rev3 --chibios-board QMK_PROTON_C
    upgrade_one_keyboard --keyboard primekb/meridian --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard projectkb/alice --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard projectkb/alice/rev1 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard projectkb/alice/rev2 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard projectkb/signature87 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard ramonimbao/squishy65 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard ramonimbao/wete --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard rart/rartlice --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard retro_75 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard rocketboard_16 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard satt/vision --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard tkc/candybar --chibios-board GENERIC_STM32_F072XB tkc/candybar/lefty tkc/candybar/righty
    upgrade_one_keyboard --keyboard tkc/candybar/lefty --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard tkc/candybar/righty --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard tkc/godspeed75 --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard tkw/stoutgat/v2/f411 --chibios-board BLACKPILL_STM32_F411
    upgrade_one_keyboard --keyboard viktus/styrka --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard vinta --chibios-board GENERIC_STM32_F042X6
    upgrade_one_keyboard --keyboard westfoxtrot/prophet --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard wolfmarkclub/wm1 --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard xelus/kangaroo --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard xelus/trinityxttkl --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard xelus/valor_frl_tkl --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard zoo/wampus --chibios-board GENERIC_STM32_F072XB
    upgrade_one_keyboard --keyboard zvecr/split_blackpill --chibios-board STM32_F103_STM32DUINO
    upgrade_one_keyboard --keyboard zvecr/zv48/f401 --chibios-board BLACKPILL_STM32_F401
    upgrade_one_keyboard --keyboard zvecr/zv48/f411 --chibios-board BLACKPILL_STM32_F411
}

leftover_chconf() {
    pushd "$qmk_firmware_dir" >/dev/null 2>&1
    for file in $(find keyboards/ -name 'chconf.h') ; do
        if [[ -z "$(grep include_next "$file")" ]] ; then
            echo $file | sed -e 's#keyboards/##g' -e 's#/chconf.h##g'
        fi
    done | sort | uniq
    popd >/dev/null 2>&1
}

leftover_halconf() {
    pushd "$qmk_firmware_dir" >/dev/null 2>&1
    for file in $(find keyboards/ -name 'halconf.h') ; do
        if [[ -z "$(grep include_next "$file")" ]] ; then
            echo $file | sed -e 's#keyboards/##g' -e 's#/halconf.h##g'
        fi
    done | sort | uniq
    popd >/dev/null 2>&1
}

leftover_mcuconf() {
    pushd "$qmk_firmware_dir" >/dev/null 2>&1
    for file in $(find keyboards/ -name 'mcuconf.h') ; do
        if [[ -z "$(grep include_next "$file")" ]] ; then
            echo $file | sed -e 's#keyboards/##g' -e 's#/mcuconf.h##g'
        fi
    done | sort | uniq
    popd >/dev/null 2>&1
}

leftover_boards() {
    { leftover_chconf ; leftover_halconf ; } | sort | uniq
}

print_leftovers() {
    leftovers=$(leftover_boards)

    echo -e "\e[1;35m$(echo "$leftovers" | wc -l) boards outstanding:\e[0m"
    for leftover in $leftovers ; do
        pushd "$qmk_firmware_dir" >/dev/null 2>&1
        echo "$leftover: $(make $leftover:default:dump_vars | grep '^BOARD=')"
        popd >/dev/null 2>&1
    done
}

#preconfigure_branch
upgrade_all_keyboards
print_leftovers