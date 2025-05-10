#!/usr/bin/env bash

set -eEuo pipefail

this_script=$(realpath "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")

qmk_firmware_dir=$(realpath "$script_dir/../qmk_firmware")

GIT_CMD="git -C $qmk_firmware_dir"

declare -a prs_to_integrate=(22887 23451 23439 20828)

cd $qmk_firmware_dir

if [[ -n "$(git -C "$qmk_firmware_dir" status -s)" ]]; then
    echo "Error: qmk_firmware directory is not clean"
    exit 1
fi

$GIT_CMD checkout develop
$GIT_CMD pull --ff-only
$GIT_CMD branch -D working-branch || true
$GIT_CMD checkout -b working-branch

for pr_id in ${prs_to_integrate[@]}; do
    $GIT_CMD fetch upstream pull/$pr_id/merge:pr-$pr_id
    $GIT_CMD merge pr-$pr_id --no-commit --squash
    $GIT_CMD commit -am "Integrate PR $pr_id"
    $GIT_CMD branch -D pr-$pr_id
done

post-checkout.sh
