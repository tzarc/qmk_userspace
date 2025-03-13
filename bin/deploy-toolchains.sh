#!/bin/bash
# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

set -eEuo pipefail

function fn_os() {
    local os_name=$(echo ${1:-} | tr 'A-Z' 'a-z')
    if [[ -z "$os_name" ]]; then
        os_name=$(uname -s | tr 'A-Z' 'a-z')
    fi
    case "$os_name" in
    *darwin* | *macos* | *apple*)
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
    *riscv64*)
        echo RV64
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

tag_name=${1:-v14.2.0-4}
TOOLCHAIN_LOCATION=${TOOLCHAIN_LOCATION:-$HOME/.local/qmk/toolchains}

echo OS: $(fn_os), Arch: $(fn_arch), tag: ${tag_name}

curl -fsSL https://api.github.com/repos/qmk/qmk_toolchains/releases/tags/${tag_name} \
    | jq -r '.assets[] | select(.name | contains("toolchain")) | .browser_download_url' \
    | grep $(fn_os)$(fn_arch) \
    | xargs curl -fsSLO

if [[ -d "$TOOLCHAIN_LOCATION" ]]; then
    echo "Removing old toolchains" &&
        rm -rf "$TOOLCHAIN_LOCATION"
fi

mkdir -p "$TOOLCHAIN_LOCATION"

echo "Extracting toolchains" &&
    tar -C "$TOOLCHAIN_LOCATION" -axf qmk_toolchains-*.tar.* --strip-components=1

rm qmk_toolchains-*.tar.*
