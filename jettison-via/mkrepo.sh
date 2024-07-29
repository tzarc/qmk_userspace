#!/usr/bin/env bash

set -eEuo pipefail

this_script=$(realpath "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")
cd $script_dir

# Generate a list of via-enabled keymaps (which aren't called `via`)
if [[ ! -e non-via-via-keymaps.txt ]]; then
    qmk find -km all -f features.via=true |
        grep -vE ':via$' \
            >non-via-via-keymaps.txt
fi

# Remove existing repo
if [[ -e "$script_dir/qmk_userspace_via" ]]; then
    rm -rf "$script_dir/qmk_userspace_via"
fi

# Setup git filter-repo paths file
echo 'regex:^keyboards/.*/keymaps/via/.*$' >filter-paths.txt
cat non-via-via-keymaps.txt |
    while read -r kbkm; do
        kb=$(echo "$kbkm" | cut -d: -f1)
        km=$(echo "$kbkm" | cut -d: -f2)
        echo >>filter-paths.txt
        echo "regex:^keyboards/${kb}/.*/keymaps/${km}/.*\$" >>filter-paths.txt
        echo "regex:^keyboards/${kb}/keymaps/${km}/.*\$" >>filter-paths.txt
        d=$(dirname ${kb})
        while [[ "$d" != "." ]]; do
            echo "regex:^keyboards/${d}/keymaps/${km}/.*\$" >>filter-paths.txt
            d=$(dirname $d)
        done
    done

# Clone the repo
git clone -b develop --no-local "$script_dir/../qmk_firmware" "$script_dir/qmk_userspace_via"

# For safety, define a git command that explicitly refers to the git repo
GIT_CMD="git -C $script_dir/qmk_userspace_via"

# Perform the filtering
cd "$script_dir/qmk_userspace_via"
git filter-repo --paths-from-file "$script_dir/filter-paths.txt" # can't use $GIT_CMD for some reason

# Swap the branch name
$GIT_CMD branch -m main

# Set up the upstream remote
$GIT_CMD remote add upstream https://github.com/qmk/qmk_userspace.git
$GIT_CMD fetch upstream

# Merge unrelated history from the main branch
$GIT_CMD merge --allow-unrelated-histories -m "Merge qmk_userspace" upstream/main

# Remove the upstream for safety
$GIT_CMD remote remove upstream

# Set up the build
QMK_USERSPACE=$(readlink -f .) qmk userspace-add all:via

# Fixup readme
cat<<EOF >README.md
# QMK Userspace -- \`all:via\`
This is the repository which contains all the VIA-enabled keymaps from \`qmk/qmk_firmware\`.
EOF

# Commit!
$GIT_CMD commit -am 'All via keymaps.'

# Push to upstream if requested
if [[ "${1:-}" == "--push" ]]; then
    $GIT_CMD remote add origin git@github.com:qmk/qmk_userspace_via.git
    $GIT_CMD push --set-upstream origin main --force
fi
