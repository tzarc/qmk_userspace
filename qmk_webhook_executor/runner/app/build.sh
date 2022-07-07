#!/bin/bash
# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later

set -eEuo pipefail

retcode=0

# Script params
unset target_repo
unset target_branch
unset pr_num
unset pr_author
unset pr_title
unset intended_sha1
while [[ -n "${1:-}" ]]; do
    case "$1" in
    --target-repo)
        shift
        target_repo="$1"
        ;;
    --target-branch)
        shift
        target_branch="$1"
        ;;
    --pr)
        shift
        pr_num="$1"
        ;;
    --sha1)
        shift
        intended_sha1="$1"
        ;;
    --author)
        shift
        pr_author="$1"
        ;;
    --title)
        shift
        pr_title="$1"
        ;;
    *)
        break
        ;;
    esac
    shift
done

# Check params
if [[ -z ${target_repo:-} ]] || [[ -z ${target_branch:-} ]] || [[ -z ${pr_num:-} ]] || [[ -z ${intended_sha1:-} ]] || [[ -z ${pr_author:-} ]] || [[ -z ${pr_title:-} ]]; then
    echo "ERROR -- Usage: $0 --target-repo <url> --target-branch <branch> --pr <pr number> --sha1 <sha1> --author <pr author> --title <pr title>" >&2
    exit 1
fi

# Command executor with log output
pcmd() {
    echo -e "\e[38;5;162m---------------------------------------------------------------------------------------------------------\e[0m" >&2
    echo -e "\e[38;5;162mExec:\e[38;5;202m $@\e[0m" >&2
    "$@"
    return $?
}

# Info helper
st_info() {
    echo -e "\e[38;5;44m[$$] Info:\e[38;5;37m $@\e[0m" >&2
}

git_hard_reset() {
    cd /__w/repo
    st_info "Resetting '/__w/repo'"
    pcmd rm .git/index.lock || true
    pcmd git remote set-url origin https://github.com/${target_repo}.git
    pcmd git fetch origin $target_branch
    pcmd git reset --hard
    pcmd git checkout -- .
    pcmd git clean -xfd
    pcmd git checkout $target_branch
    pcmd git reset --hard origin/$target_branch
    pcmd rm -rf lib/*
    pcmd git checkout -- .
    pcmd make git-submodule
    pcmd git clean -xfd
}

prepare_repo() {
    local retcode=0
    cd /__w/repo

    if [[ ! -d ".git" ]]; then
        st_info "Cloning repository"
        pcmd git clone https://github.com/${target_repo}.git /__w/repo
    fi

    st_info "Setting commit parameters"
    pcmd git config merge.renamelimit 200000
    pcmd git config user.name "Tzarc CI"
    pcmd git config user.email runner@ci.tzarc.io

    git_hard_reset

    st_info "Fetching PR #${pr_num} info"
    pcmd git fetch origin pull/${pr_num}/head:pr_${pr_num}

    st_info "Saving list of keyboards (pre-merge)"
    #pcmd qmk list-keyboards >/__w/keyboards-before.txt
    find keyboards/ -type d -and -path '*/keymaps/*' | sed -e 's@^keyboards/@@g' -e 's@/keymaps/@ @g' >/__w/keymaps-before.txt

    st_info "Applying PR to $target_branch"
    { pcmd git merge --no-commit --squash ${intended_sha1} 2>&1 || touch /__w/failed.$intended_sha1; } | tee /__w/build_output.txt
    if [[ -e /__w/failed.$intended_sha1 ]]; then
        retcode=1
    fi

    if [[ $retcode -eq 0 ]]; then
        st_info "Saving list of keyboards (post-merge)"
        #pcmd qmk list-keyboards >/__w/keyboards-after.txt
        find keyboards/ -type d -and -path '*/keymaps/*' | sed -e 's@^keyboards/@@g' -e 's@/keymaps/@ @g' >/__w/keymaps-after.txt

        st_info "Preparing build"
        { pcmd make git-submodule 2>&1 || touch /__w/failed.$intended_sha1; } | tee -a /__w/build_output.txt
        { pcmd git status -s 2>&1 || touch /__w/failed.$intended_sha1; } | tee -a /__w/build_output.txt
    fi

    if [[ -e /__w/failed.$intended_sha1 ]]; then
        retcode=1
    fi

    st_info "RC=$retcode"
    return $retcode
}

to_build=()

collect_new_keymaps() {
    cd /__w/repo

    # Do some gymnastics to determine if keymaps are at a higher directory than the keyboard -- caters for keymaps at the same level as rev1/rev2 etc. -- we build both
    while read keyboard; do
        while read kb; do
            while read keymap; do
                to_build+=("${kb}:${keymap}")
            done < <(pcmd diff /__w/keymaps-before.txt /__w/keymaps-after.txt | pcmd grep $keyboard | pcmd awk '/^>/ {print $3}' | sort | uniq)
        done < <(pcmd qmk list-keyboards | pcmd grep $keyboard)
    done < <(pcmd diff /__w/keymaps-before.txt /__w/keymaps-after.txt | pcmd awk '/^>/ {print $2}' | sort | uniq)

    return 0
}

collect_modified_keymaps() {
    cd /__w/repo

    # Same gymnastics as above.
    while read keyboard; do
        while read kb; do
            while read keymap; do
                to_build+=("${kb}:${keymap}")
            done < <(pcmd git status -s | pcmd awk '/^M/ {print $2}' | pcmd grep $keyboard | pcmd sed -e 's@.*/keymaps/@@g' -e 's@/.*@@g' | sort | uniq)
        done < <(pcmd qmk list-keyboards | pcmd grep $keyboard)
    done < <(pcmd git status -s | pcmd awk '/^M.*\/keymaps\// {print $2}' | pcmd sed -e 's@^keyboards/@@g' -e 's@/keymaps.*@@g' | sort | uniq)
}

build_keymaps() {
    local retcode=0
    cd /__w/repo

    while read target; do
        st_info "Making ${target}..."
        { pcmd make --no-print-directory -j$(nproc) -O ${target} 2>&1 || touch /__w/failed.$intended_sha1; } | tee -a /__w/build_output.txt
        if [[ -e /__w/failed.$intended_sha1 ]]; then
            retcode=1
        fi
    done < <(for tmp in ${to_build[@]}; do echo $tmp; done | sort | uniq)

    return $retcode
}

export script_rc=1
cleanup() {
    cd /__w/repo

    # Reset the repo
    git_hard_reset

    st_info "Removing historical submodules"
    [ -e lib/ugfx ] && pcmd rm -rf lib/ugfx || true
    [ -e lib/pico-sdk ] && pcmd rm -rf lib/pico-sdk || true
    [ -e lib/chibios-contrib/ext/mcux-sdk ] && pcmd rm -rf lib/chibios-contrib/ext/mcux-sdk || true

    st_info "Removing branch"
    pcmd git branch -D pr_${pr_num}
    pcmd git gc --auto

    exit $script_rc
}
trap cleanup EXIT HUP INT

ctlchars2html() {
    cat - | sed -e 's@/__w/repo@%QMK_FIRMWARE%@g' | ansi2html --bg=dark --palette=linux --body-only 2>/dev/null | sed -e 's@\r@@g' -e 's@\n\n@\n@g' -e 's@\n\n@\n@g' -e 's@\n\n@\n@g' -e 's@\n\n@\n@g'
}

main() {
    # Clear out files
    pcmd rm /__w/*.{txt,html} || true
    pcmd rm /__w/failed.* || true

    # Prepare the repo for build, including a test merge into the target branch
    if ! prepare_repo; then retcode=1; fi

    if [[ $retcode -eq 0 ]] && [[ ! -e /__w/failed.$intended_sha1 ]]; then
        # Work out if there are any new keymaps to build
        collect_new_keymaps
        # Work out if there are any modified keymaps to build
        collect_modified_keymaps

        if [[ ${#to_build[@]} -gt 0 ]]; then
            # Attempt to build any keymaps that were added
            if ! build_keymaps; then retcode=1; fi
        fi

        if [[ ! -z $(pcmd git status -s | pcmd grep ' \(builddefs\|drivers\|lib\|platforms\|quantum\|tmk_core\)/') ]]; then
            # There were changes in core areas, do a multibuild
            { pcmd qmk multibuild -j $(nproc) -km default 2>&1 || touch /__w/failed.$intended_sha1; } | tee -a /__w/build_output.txt
            { pcmd qmk multibuild -j $(nproc) -km via 2>&1 || touch /__w/failed.$intended_sha1; } | tee -a /__w/build_output.txt
        fi
    fi

    # Copy out all of the failures
    cp /__w/repo/.build/failed.* /__w/ || true

    st_info "Generating output file"
    cat <<EOF >/__w/${intended_sha1}.html
<!DOCTYPE html>
<html lang='en'><head>
<meta charset="utf-8"/>
<style type='text/css'>
$(ansi2html --bg=dark --palette=linux --css-only 2>/dev/null | sed -e 's@\r@@g' -e 's@\n\n@\n@g' -e 's@\n\n@\n@g' -e 's@\n\n@\n@g' -e 's@\n\n@\n@g')
pre { font-size: 80%; }
h1, h2, h3, h4, h5, h6, pre { font-family: 'Iosevka Term', 'Iosevka Fixed', Consolas, Menlo, 'Courier New', monospace; }
a { color: #FF0; font-weight: bold; }
a:visited { color: #FF0; }
a:hover { color: #F00; }
</style>
<title>$target_repo -- $target_branch -- PR #$pr_num -- $intended_sha1</title>
</head><body class='f9 b9'>
<h3><a href="https://github.com/$target_repo/pull/$pr_num">PR #$pr_num</a> (at $intended_sha1)</h3>
<h4><span style="color: #666">&gt;&gt;&gt; Merge to:</span> $target_branch</h4>
<h4><span style="color: #666">&gt;&gt;&gt; Author:</span> $pr_author</h4>
<h4><span style="color: #666">&gt;&gt;&gt; Title:</span> $pr_title</h4>
EOF

    # Work out whether there were any failures
    if [[ $retcode -eq 0 ]] && [[ -z "$(ls /__w/failed.*)" ]]; then
        st_info Build succeeded
        cat <<EOF >>/__w/${intended_sha1}.html
<h4>Build <span style="color: #00FF00">successful</span></h4>
EOF
    else
        st_info Build failed
        script_rc=1
        cat <<EOF >>/__w/${intended_sha1}.html
<h4>Build <span style="color: #FF0000">failed</span></h4>
EOF
    fi

    cat <<EOF >>/__w/${intended_sha1}.html
<hr/>
<pre>
$(cat /__w/build_output.txt | ctlchars2html)
$(cat /__w/failed.* | ctlchars2html)
</pre>
</body></html>
EOF
}

main
script_rc=0
