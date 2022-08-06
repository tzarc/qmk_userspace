#!/bin/bash

this_script="$(readlink -f "${BASH_SOURCE[0]}")"
script_dir="$(readlink -f "$(dirname "$this_script")")"

docker rm qmk_develop_runner || true
echo > "$script_dir/.repo-hash"
renc -i "$HOME/.ssh/id_ed25519" < "$script_dir/create-builder.sh.enc" | bash