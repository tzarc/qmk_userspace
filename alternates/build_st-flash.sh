#!/bin/bash
# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

set -e

umask 022
this_script=$(readlink -f "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")

sudo apt install -y build-essential cmake libusb-1.0 libusb-1.0-0-dev

[ -d "$script_dir/stlink-repo/.git" ] \
	|| git clone https://github.com/texane/stlink.git "$script_dir/stlink-repo"

[ -d "$script_dir/stlink-build" ] \
	|| mkdir -p "$script_dir/stlink-build"

cd "$script_dir/stlink-repo"
git checkout -- .
git clean -xfd
git pull
git reset --hard origin/master

cd "$script_dir/stlink-build"
cmake -DCMAKE_BUILD_TYPE=Release ../stlink-repo
make -j$(nproc)
sudo make install
sudo ldconfig

rm -rf "$script_dir/stlink-build" "$script_dir/stlink-repo"
