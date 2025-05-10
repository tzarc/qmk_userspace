#!/bin/bash
# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

set -e

umask 022
this_script=$(readlink -f "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")

echo "Make sure deb-src entries are enabled in /etc/apt/sources.list"
sudo apt build-dep -y libusb-1.0-0 dfu-util
sudo apt install -y build-essential cmake libusb-1.0 libusb-1.0-0-dev mingw-w64

[ -d "$script_dir/dfu-util-repo/.git" ] ||
    git clone https://git.code.sf.net/p/dfu-util/dfu-util "$script_dir/dfu-util-repo"

[ -d "$script_dir/dfu-util-build-linux" ] ||
    mkdir -p "$script_dir/dfu-util-build-linux"

[ -d "$script_dir/libusb-repo/.git" ] ||
    git clone https://github.com/libusb/libusb.git "$script_dir/libusb-repo"

cd "$script_dir/dfu-util-repo"
git checkout -- .
git clean -xfd
git pull
git reset --hard origin/master
./autogen.sh

cd "$script_dir/dfu-util-build-linux"
../dfu-util-repo/configure
make -j$(nproc)
sudo make install
sudo ldconfig

rm -rf "$script_dir/dfu-util-build-linux" "$script_dir/dfu-util-repo"
