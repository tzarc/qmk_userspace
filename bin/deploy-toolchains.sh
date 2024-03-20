#!/bin/bash
# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

set -eEuo pipefail

function fn_os() {
    local os_name=$(uname -s | tr 'A-Z' 'a-z')
    case "$os_name" in
    *darwin* | *macos*)
        echo macos
        ;;
    *windows* | *mingw* | *w64*)
        echo windows
        ;;
    *linux*)
        echo linux
        ;;
    *)
        echo "unknown operating system '$os_name'" >&2
        exit 1
        ;;
    esac
}

function fn_arch() {
    local arch_name=$(uname -m | tr 'A-Z' 'a-z')
    case "$arch_name" in
    *arm64* | *aarch64*)
        echo ARM64
        ;;
    *x86_64* | *x64*)
        echo X64
        ;;
    *)
        echo unknown architecture $arch_name >&2
        exit 1
        ;;
    esac
}

tag_name=${1:-latest}

echo OS: $(fn_os), Arch: $(fn_arch), tag: ${tag_name}

[[ -f qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalARM.tar.xz ]] ||
    {
        echo "Downloading ARM toolchain" &&
            curl -fsSLO https://github.com/qmk/qmk_toolchains/releases/download/${tag_name}/qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalARM.tar.xz
    }
[[ -f qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalAVR.tar.xz ]] ||
    {
        echo "Downloading AVR toolchain" &&
            curl -fsSLO https://github.com/qmk/qmk_toolchains/releases/download/${tag_name}/qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalAVR.tar.xz
    }
[[ -f qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalRV32.tar.xz ]] ||
    {
        echo "Downloading RISC-V toolchain" &&
            curl -fsSLO https://github.com/qmk/qmk_toolchains/releases/download/${tag_name}/qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalRV32.tar.xz
    }

if [[ -d "$HOME/.local/qmk/toolchains" ]]; then
    echo "Removing old toolchains" &&
        rm -rf "$HOME/.local/qmk/toolchains"
fi

mkdir -p "$HOME/.local/qmk/toolchains"

echo "Extracting ARM toolchain" &&
    tar -C "$HOME/.local/qmk/toolchains" -xf qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalARM.tar.xz --strip-components=1
echo "Extracting AVR toolchain" &&
    tar -C "$HOME/.local/qmk/toolchains" -xf qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalAVR.tar.xz --strip-components=1
echo "Extracting RISC-V toolchain" &&
    tar -C "$HOME/.local/qmk/toolchains" -xf qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalRV32.tar.xz --strip-components=1

if [[ $(fn_os) == "linux" ]]; then
    [[ -f qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_$(fn_os)$(fn_arch).tar.xz ]] ||
        {
            echo "Downloading $(fn_os)$(fn_arch) toolchain" &&
                curl -fsSLO https://github.com/qmk/qmk_toolchains/releases/download/${tag_name}/qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_$(fn_os)$(fn_arch).tar.xz
        }
    echo "Extracting $(fn_os)$(fn_arch) toolchain" &&
        tar -C "$HOME/.local/qmk/toolchains" -xf qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_$(fn_os)$(fn_arch).tar.xz --strip-components=1
fi
