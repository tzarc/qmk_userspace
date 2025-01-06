#!/bin/bash
# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

set -eEuo pipefail

this_script=$(realpath "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")
cd $script_dir/..

test_distro() {
    local distro=$1
    echo -e '\e[38;5;11m################################################################\e[0m'
    echo -e "\e[38;5;6mTesting: \e[38;5;14m$distro\e[0m"
    echo -e '\e[38;5;11m################################################################\e[0m'
    docker run --rm -it -v $HOME/.local/qmk/toolchains:/opt/qmk $distro bash -c "export PATH=\"/opt/qmk/bin:\$PATH\"; avr-gcc -v && arm-none-eabi-gcc -v && riscv32-unknown-elf-gcc -v"
}

test_distro debian:12
test_distro debian:11
test_distro debian:10

test_distro fedora:36
test_distro fedora:37
test_distro fedora:38
test_distro fedora:39
test_distro fedora:40

test_distro archlinux:latest
