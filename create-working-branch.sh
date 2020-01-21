#!/bin/bash

set -eEuo pipefail

this_script="$(readlink -f "${BASH_SOURCE[0]}")"
script_dir="$(readlink -f "$(dirname "$this_script")")"

declare -a prs_to_apply
prs_to_apply+=(4226)
prs_to_apply+=(7274)
prs_to_apply+=(7766)
prs_to_apply+=(7781)

pushd "$script_dir/qmk_firmware"

git clean -xfd
git checkout -- .
git reset --hard
git fetch --all
git checkout chibios-upgrade
git reset --hard origin/chibios-upgrade
git branch --force cyclone-merged-prs origin/chibios-upgrade
git checkout cyclone-merged-prs

for pr in ${prs_to_apply[@]} ; do
    curl -qL https://github.com/qmk/qmk_firmware/pull/${pr}.diff > "$script_dir/PR-${pr}.patch"
    patch -Np1 < "$script_dir/PR-${pr}.patch" || true
done

find . \( -name '*.rej' -or -name '*.orig' \) -delete

make git-submodule

git add -A
git commit -am 'Rebuild branch "cyclone-merged-prs"'
git push origin cyclone-merged-prs --set-upstream --force-with-lease

popd
