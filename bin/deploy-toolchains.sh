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

tag_name=${1:-v14.1.0-4}

echo OS: $(fn_os), Arch: $(fn_arch), tag: ${tag_name}

curl -fsSL https://api.github.com/repos/qmk/qmk_toolchains/releases/tags/${tag_name} \
    | jq -r '.assets[] | select(.name | contains("toolchain")) | .browser_download_url' \
    | grep $(fn_os)$(fn_arch) \
    | xargs curl -fsSLO

if [[ -d "$HOME/.local/qmk/toolchains" ]]; then
    echo "Removing old toolchains" &&
        rm -rf "$HOME/.local/qmk/toolchains"
fi

mkdir -p "$HOME/.local/qmk/toolchains"

echo "Extracting toolchains" &&
    tar -C "$HOME/.local/qmk/toolchains" -xf qmk_toolchains-*.tar.xz --strip-components=1

rm qmk_toolchains-*.tar.xz
