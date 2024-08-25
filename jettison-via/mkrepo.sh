#!/usr/bin/env bash

set -xeEuo pipefail

this_script=$(realpath "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")
cd $script_dir

# Remove existing repo
if [[ -e "$script_dir/qmk_userspace_via" ]]; then
    rm -rf "$script_dir/qmk_userspace_via"
fi

export QMK_USERSPACE="$script_dir/qmk_userspace_via"

# Clone the repo
git clone -b via-keymaps-still-present --no-local "$script_dir/../qmk_firmware" "$script_dir/qmk_userspace_via"

# For safety, define a git command that explicitly refers to the git repo
GIT_CMD="git -C $script_dir/qmk_userspace_via"

# If SSH signing is available, use it.
$GIT_CMD sshsign || true

# Swap to the correct directory
cd "$script_dir/qmk_userspace_via"

# Generate a list of via-enabled keymaps (which aren't called `via`)
if [[ ! -e "$script_dir/non-via-via-keymaps.txt" ]]; then
    qmk find -km all -f features.via=true |
        grep -vE ':via$' \
            >"$script_dir/non-via-via-keymaps.txt"
fi

# Setup git filter-repo paths file
echo 'regex:^keyboards/.*/keymaps/via/.*$' >"$script_dir/filter-paths.txt"
cat "$script_dir/non-via-via-keymaps.txt" |
    sort |
    xargs "$script_dir/keymap_locations.py" |
    uniq |
    sed -e 's@^@regex:^@g' -e 's@$@/.*$@g' \
        >>"$script_dir/filter-paths.txt"

# Perform the filtering
git filter-repo --paths-from-file "$script_dir/filter-paths.txt" # can't use $GIT_CMD for some reason

# Swap the branch name
$GIT_CMD switch -c main

# Set up the upstream remote
$GIT_CMD remote add upstream https://github.com/qmk/qmk_userspace.git
$GIT_CMD fetch upstream

# Merge unrelated history from the main branch
$GIT_CMD merge --allow-unrelated-histories -m "Merge qmk_userspace" upstream/main

# Remove the upstream for safety
$GIT_CMD remote remove upstream

# Set up the build
qmk userspace-add all:via

# Fixup readme
cat <<EOF >README.md
# QMK Userspace -- \`all:via\`
This is the repository which contains all the VIA-enabled keymaps from \`qmk/qmk_firmware\`.

This repository is currently in the process of being transferred into the VIA team's control, and does not accept any PRs until that occurs.
EOF

# Commit!
$GIT_CMD commit -am 'All via keymaps.'

# Apply patches in the script dir
ls -1 "$script_dir/"*.patch | sort | while read -r patch; do
    $GIT_CMD apply "$patch"
    $GIT_CMD commit -am "Apply patch $(basename $patch .patch)"
done

# Push to upstream if requested
if [[ "${1:-}" == "--push" ]]; then
    $GIT_CMD remote add origin git@github.com:qmk/qmk_userspace_via.git
    $GIT_CMD push --set-upstream origin main --force
fi
