#!/bin/bash
# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

set -eEuo pipefail

this_script=$(realpath "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")
cd $script_dir/..

if [ "${1:-}" == "inner" ]; then

    # This is run inside the docker container
    export PATH="/opt/qmk/bin:$PATH"

    avr-gcc -v 2>&1
    arm-none-eabi-gcc -v 2>&1
    riscv32-unknown-elf-gcc -v 2>&1

else

    test_distro() {
        local distro=$1
        echo -e '\e[38;5;11m################################################################\e[0m'
        echo -e "\e[38;5;6mTesting: \e[38;5;14m$distro\e[0m"
        echo -e '\e[38;5;11m################################################################\e[0m'
        docker run --rm -it -v $(realpath .distrib):/opt/qmk -v $(realpath "$this_script"):/test-toolchains.sh $distro bash -c "/test-toolchains.sh inner"
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
fi
