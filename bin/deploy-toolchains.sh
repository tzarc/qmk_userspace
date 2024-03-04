#!/bin/bash
# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

function fn_os() {
    local os_name=$(echo ${1:-} | tr 'A-Z' 'a-z')
    if [[ -z "$os_name" ]]; then
        os_name=$(uname -s | tr 'A-Z' 'a-z')
    fi
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
    *none* | *unknown* | *picolibc* | *nano*)
        echo baremetal
        ;;
    *)
        echo unknown
        ;;
    esac
}

function fn_arch() {
    local arch_name=$(echo ${1:-} | tr 'A-Z' 'a-z')
    if [[ -z "$arch_name" ]]; then
        arch_name=$(uname -m | tr 'A-Z' 'a-z')
    fi
    case "$arch_name" in
    *arm64* | *aarch64*)
        echo ARM64
        ;;
    *arm*)
        echo ARM
        ;;
    *riscv32*)
        echo RV32
        ;;
    *avr*)
        echo AVR
        ;;
    *x86_64* | *x64*)
        echo X64
        ;;
    *)
        echo unknown
        ;;
    esac
}

echo OS: $(fn_os), Arch: $(fn_arch)

[[ -f qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalARM.tar.xz ]] || curl -fsSLO https://github.com/tzarc/qmk_toolchains/releases/download/latest/qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalARM.tar.xz
[[ -f qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalAVR.tar.xz ]] || curl -fsSLO https://github.com/tzarc/qmk_toolchains/releases/download/latest/qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalAVR.tar.xz
[[ -f qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalRV32.tar.xz ]] || curl -fsSLO https://github.com/tzarc/qmk_toolchains/releases/download/latest/qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalRV32.tar.xz

if [[ -d "$HOME/.local/qmk/toolchains" ]]; then
    rm -rf "$HOME/.local/qmk/toolchains"
fi

mkdir -p "$HOME/.local/qmk/toolchains"

tar -C "$HOME/.local/qmk/toolchains" -xf qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalARM.tar.xz --strip-components=1
tar -C "$HOME/.local/qmk/toolchains" -xf qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalAVR.tar.xz --strip-components=1
tar -C "$HOME/.local/qmk/toolchains" -xf qmk_toolchain-gcc13.2.0-host_$(fn_os)$(fn_arch)-target_baremetalRV32.tar.xz --strip-components=1
