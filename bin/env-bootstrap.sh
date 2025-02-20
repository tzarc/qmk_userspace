#!/usr/bin/env sh

# Install `uv` (or update as necessary)
if [ -n "$(command -v curl 2>/dev/null || true)" ]; then
    curl -LsSf https://astral.sh/uv/install.sh | sh
elif [ -n "$(command -v wget 2>/dev/null || true)" ]; then
    wget -qO- https://astral.sh/uv/install.sh | sh
else
    echo "Please install 'curl' or 'wget' to continue."
    exit 1
fi

# Set up the paths for any of the locations `uv` expects
[ ! -d "$XDG_BIN_HOME" ] || export PATH="$XDG_BIN_HOME:$PATH"
[ ! -d "$XDG_DATA_HOME/../bin" ] || export PATH="$XDG_DATA_HOME/../bin:$PATH"
[ ! -d "$HOME/.local/bin" ] || export PATH="$HOME/.local/bin:$PATH"

if [ -n "${UV_INSTALL_DIR:-}" ]; then
    export PATH="$UV_INSTALL_DIR/bin:$UV_INSTALL_DIR:$PATH" # cater for both "flat" and "hierarchical" installs of `uv`
fi

# Install the QMK CLI
uv tool install --force --with pip --upgrade --python 3.13 qmk

# QMK is installed to...
QMK_TOOL_INSTALLDIR="$(uv tool dir)/qmk"

# Activate the environment
. "$QMK_TOOL_INSTALLDIR/bin/activate"

# Install the QMK dependencies
uv pip install --upgrade -r https://raw.githubusercontent.com/qmk/qmk_firmware/refs/heads/master/requirements.txt
uv pip install --upgrade -r https://raw.githubusercontent.com/qmk/qmk_firmware/refs/heads/master/requirements-dev.txt

# Deactivate the environment
deactivate

# Notify the user that they may need to restart their shell to get the `qmk` command
echo
echo "You may need to restart your shell to gain access to the 'qmk' command."
echo "Alternatively, add "$(dirname "$(command -v qmk)")" to your \$PATH:"
echo "    export PATH=\"$(dirname "$(command -v qmk)"):\$PATH\""
