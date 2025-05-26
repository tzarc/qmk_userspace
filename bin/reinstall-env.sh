#!/usr/bin/env bash

this_script=$(realpath "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")

curl -fsSL https://install.qmk.fm/ | CONFIRM=1 SKIP_PACKAGE_MANAGER=1 SKIP_UV=1 SKIP_UDEV_RULES=1 sh

"${script_dir}/post-checkout.sh"
