#!/usr/bin/env sh
# Copyright 2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

{ # this ensures the entire script is downloaded #
    set -eu

    WORKING_DIR="$(pwd)"

    download_url() {
        local url=$1
        local filename=${2:-$(basename "$url")}
        local quiet=''
        if [ -n "$(command -v curl 2>/dev/null || true)" ]; then
            [ "$filename" = "-" ] && quiet='-s' || echo "Downloading '$url' => '$filename'" >&2
            curl -LSf $quiet -o "$filename" "$url"
        elif [ -n "$(command -v wget 2>/dev/null || true)" ]; then
            [ "$filename" = "-" ] && quiet='-q' || echo "Downloading '$url' => '$filename'" >&2
            wget $quiet "-O$filename" "$url"
        else
            echo "Please install 'curl' to continue." >&2
            exit 1
        fi
    }

    github_api_call() {
        local url="$1"
        local token="${GITHUB_TOKEN:-${GH_TOKEN:-}}"
        if [ -n "${token:-}" ]; then
            if [ -n "$(command -v curl 2>/dev/null || true)" ]; then
                curl -fsSL -H "Authorization: token $token" -H "Accept: application/vnd.github.v3+json" "https://api.github.com/$url"
            elif [ -n "$(command -v wget 2>/dev/null || true)" ]; then
                wget -q --header="Authorization: token $token" --header="Accept: application/vnd.github.v3+json" "https://api.github.com/$url" -O -
            fi
        else
            download_url "https://api.github.com/$url" -
        fi
    }

    fn_os() {
        local os_name=$(echo ${1:-} | tr 'A-Z' 'a-z')
        if [ -z "$os_name" ]; then
            os_name=$(uname -s | tr 'A-Z' 'a-z')
        fi
        case "$os_name" in
        *darwin* | *macos* | *apple*)
            echo macos
            ;;
        *windows* | *mingw* | *msys*)
            echo windows
            ;;
        *linux*)
            echo linux
            ;;
        *)
            echo unknown
            ;;
        esac
    }

    fn_arch() {
        local arch_name=$(echo ${1:-} | tr 'A-Z' 'a-z')
        if [ -z "$arch_name" ]; then
            arch_name=$(uname -m | tr 'A-Z' 'a-z')
        fi
        case "$arch_name" in
        *arm64* | *aarch64*)
            echo ARM64
            ;;
        *riscv64*)
            echo RV64
            ;;
        *x86_64* | *x64*)
            echo X64
            ;;
        *)
            echo unknown
            ;;
        esac
    }

    fetch_tarball() {
        local url="$1"
        local target_file="$2"
        local name local_name
        name=$(basename "$url")
        # Strip version segment (e.g. "gcc15.2.0-") before "host_" for local lookup
        local_name=$(echo "$name" | sed 's/-[^-]*[0-9][^-]*-host_/-host_/')
        if [ -f "$WORKING_DIR/$local_name" ]; then
            echo "Using local '$local_name'..." >&2
            cp "$WORKING_DIR/$local_name" "$target_file"
        else
            download_url "$url" "$target_file"
        fi
    }

    install_toolchains() {
        local host_key="host_$(fn_os)$(fn_arch)"

        local all_urls
        all_urls=$(github_api_call repos/tzarc/qmk_toolchains/releases/tags/latest |
            grep -oE '"browser_download_url": "[^"]+"' |
            grep -oE 'https://[^"]+')

        local host_urls
        host_urls=$(echo "$all_urls" | grep -F "$host_key" || true)

        if [ -z "$host_urls" ]; then
            echo "No toolchain assets found for host '$host_key'." >&2
            exit 1
        fi

        echo "$host_urls" | while IFS= read -r url; do
            local target_file="$QMK_DISTRIB_DIR/$(basename "$url")"
            fetch_tarball "$url" "$target_file"
            echo "Extracting '$(basename "$url")' to '$QMK_DISTRIB_DIR'..." >&2
            zstdcat "$target_file" | tar xf - -C "$QMK_DISTRIB_DIR" --strip-components=1
            rm -f "$target_file"
        done
    }

    install_flashing_tools() {
        local osarch="$(fn_os)$(fn_arch)"

        # Special case for WSL
        if [ -n "${WSL_INSTALL:-}" ] || [ -n "${WSL_DISTRO_NAME:-}" ] || [ -f /proc/sys/fs/binfmt_misc/WSLInterop ]; then
            osarch="windowsWSL"
        fi

        local all_urls
        all_urls=$(github_api_call repos/tzarc/qmk_flashutils/releases/tags/latest |
            grep -oE '"browser_download_url": "[^"]+"' |
            grep -oE 'https://[^"]+')

        local flashutils_url
        flashutils_url=$(echo "$all_urls" | grep -E "qmk_flashutils-.*${osarch}" || true)
        if [ -z "$flashutils_url" ]; then
            echo "No flashing tools found for '$osarch'." >&2
            exit 1
        fi

        mkdir -p "$QMK_DISTRIB_DIR/bin"

        local target_file="$QMK_DISTRIB_DIR/$(basename "$flashutils_url")"
        fetch_tarball "$flashutils_url" "$target_file"
        echo "Extracting flashing tools to '$QMK_DISTRIB_DIR/bin'..." >&2
        zstdcat "$target_file" | tar xf - -C "$QMK_DISTRIB_DIR/bin"
        rm -f "$target_file"
    }

    if [ -z "${QMK_DISTRIB_DIR:-}" ]; then
        echo "QMK_DISTRIB_DIR is not set." >&2
        exit 1
    fi

    mkdir -p "$QMK_DISTRIB_DIR"

    install_toolchains
    install_flashing_tools

    echo >&2
    echo "tzarc distrib installation complete." >&2
    echo "Toolchains and flashing utilities installed to '$QMK_DISTRIB_DIR'." >&2

} # this ensures the entire script is downloaded #
