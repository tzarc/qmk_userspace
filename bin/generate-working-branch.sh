#!/usr/bin/env bash

set -eEuo pipefail

this_script=$(realpath "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")

qmk_firmware_dir=$(realpath "$script_dir/../qmk_firmware")

declare -a prs_to_integrate=(22887 23451 23439 20828)

cd $qmk_firmware_dir

if [[ ! -z "$(git status -s)" ]]; then
  echo "Error: qmk_firmware directory is not clean"
  exit 1
fi

git checkout develop
git pull --ff-only
git branch -D working-branch || true
git checkout -b working-branch

for pr_id in ${prs_to_integrate[@]}; do
  git fetch upstream pull/$pr_id/merge:pr-$pr_id
  git merge pr-$pr_id --squash
  git commit -m "Integrate PR $pr_id"
  git branch -D pr-$pr_id
done

post-checkout.sh
