#!/usr/bin/env sh

set -eu

# Windows doesn't like `/tmp` so we need to set a different temporary directory
# and also set the `UV_INSTALL_DIR` to a location that doesn't pollute the user's
# home directory.
if [ "$(uname -o 2>/dev/null || true)" = "Msys" ]; then
    export TMPDIR="$(cygpath -w "$TMP")"
    export UV_INSTALL_DIR=/opt/uv
fi

function download_url() {
    if [ -n "$(command -v curl 2>/dev/null || true)" ]; then
        curl -LsSf "$1"
    elif [ -n "$(command -v wget 2>/dev/null || true)" ]; then
        wget -qO- "$1"
    else
        echo "Please install 'curl' or 'wget' to continue."
        exit 1
    fi
}

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

# Install `uv` (or update as necessary)
download_url https://astral.sh/uv/install.sh | TMPDIR=${TMPDIR:-} UV_INSTALL_DIR=${UV_INSTALL_DIR:-} sh

# Set up the paths for any of the locations `uv` expects
if [ -n "${XDG_BIN_HOME:-}" ]; then
    export PATH="$XDG_BIN_HOME:$PATH"
fi
if [ -n "${XDG_DATA_HOME:-}" ]; then
    export PATH="$XDG_DATA_HOME/../bin:$PATH"
fi
[ ! -d "$HOME/.local/bin" ] || export PATH="$HOME/.local/bin:$PATH"

if [ -n "${UV_INSTALL_DIR:-}" ]; then
    export PATH="$UV_INSTALL_DIR/bin:$UV_INSTALL_DIR:$PATH" # cater for both "flat" and "hierarchical" installs of `uv`
fi

# Install the QMK CLI
uv tool install --force --with pip --upgrade --python 3.13 qmk

# QMK is installed to...
QMK_TOOL_INSTALLDIR="$(uv tool dir)/qmk"

# Convert it to a unix-style path if we're on Windows/Msys2
if [ "$(uname -o 2>/dev/null || true)" = "Msys" ]; then
    QMK_TOOL_INSTALLDIR="$(cygpath -u "$QMK_TOOL_INSTALLDIR")"
fi

# Activate the environment
if [ -e "$QMK_TOOL_INSTALLDIR/bin" ] ; then
    . "$QMK_TOOL_INSTALLDIR/bin/activate"
elif [ -e "$QMK_TOOL_INSTALLDIR/Scripts" ]; then
    . "$QMK_TOOL_INSTALLDIR/Scripts/activate"
else
    echo "Could not find the QMK environment to activate."
    exit 1
fi

# Install the QMK dependencies
uv pip install --upgrade -r https://raw.githubusercontent.com/qmk/qmk_firmware/refs/heads/master/requirements.txt
uv pip install --upgrade -r https://raw.githubusercontent.com/qmk/qmk_firmware/refs/heads/master/requirements-dev.txt

# Deactivate the environment
deactivate

# Work out where we want to install the toolchains
toolchains_location=${QMK_TOOLCHAIN_DIR:-$HOME/.local/qmk/toolchains}

# Get the latest toolchain release from https://github.com/qmk/qmk_toolchains
latest_toolchains_release=$(download_url https://api.github.com/repos/qmk/qmk_toolchains/releases/latest | grep -oE '"tag_name": "[^"]+' | grep -oE '[^"]+$')
# Download the specific release asset with a matching keyword
toolchain_url=$(download_url https://api.github.com/repos/qmk/qmk_toolchains/releases/tags/$latest_toolchains_release | grep -oE '"browser_download_url": "[^"]+"' | grep -oE 'https://[^"]+' | grep $(fn_os)$(fn_arch))
if [ -z "$toolchain_url" ]; then
    echo "No toolchain found for this OS/Arch combination." >&2
    exit 1
fi

# Clear out the toolchains
if [[ -d "$toolchains_location" ]]; then
    echo "Removing old toolchains"
    rm -rf "$toolchains_location"
fi
mkdir -p "$toolchains_location"

# Download the toolchain release to the toolchains location
echo "Downloading toolchain"
download_url "$toolchain_url" > "$toolchains_location/$(basename "$toolchain_url")"

# Extract the toolchain
echo "Extracting toolchain"
tar xf "$toolchains_location/$(basename "$toolchain_url")" -C "$toolchains_location" --strip-components=1

# Get the latest flashing tools release from https://github.com/qmk/qmk_flashutils
latest_flashutils_release=$(download_url https://api.github.com/repos/qmk/qmk_flashutils/releases/latest | grep -oE '"tag_name": "[^"]+' | grep -oE '[^"]+$')
# Download the specific release asset with a matching keyword
flashutils_url=$(download_url https://api.github.com/repos/qmk/qmk_flashutils/releases/tags/$latest_flashutils_release | grep -oE '"browser_download_url": "[^"]+"' | grep -oE 'https://[^"]+' | grep $(fn_os)$(fn_arch))
if [ -z "$flashutils_url" ]; then
    echo "No flashing tools found for this OS/Arch combination." >&2
    exit 1
fi

# Download the flashing tools release to the toolchains location
echo "Downloading flashing tools"
download_url "$flashutils_url" > "$toolchains_location/$(basename "$flashutils_url")"

# Extract the flashing tools
echo "Extracting flashing tools"
tar xf "$toolchains_location/$(basename "$flashutils_url")" -C "$toolchains_location/bin"

# Notify the user that they may need to restart their shell to get the `qmk` command
echo
echo "You may need to restart your shell to gain access to the 'qmk' command."
echo "Alternatively, add "$(dirname "$(command -v qmk)")" to your \$PATH:"
echo "    export PATH=\"$(dirname "$(command -v qmk)"):\$PATH\""
