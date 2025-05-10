#!/usr/bin/env python3
# Copyright 2024-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later
import os
import sys
import fileinput
import shlex
from unittest.mock import patch
from pathlib import Path

qmk_firmware_path = Path(__file__).parent / Path("..") / "qmk_firmware"
qmk_cli_path = qmk_firmware_path / "lib/python"

sys.path.append(str(qmk_cli_path))

orig_cwd = os.getcwd()
os.environ["ORIG_CWD"] = str(orig_cwd)
os.chdir(qmk_firmware_path)
os.environ["QMK_HOME"] = str(qmk_firmware_path)


def unload_qmk_cli() -> None:
    cycles = 0

    def mods():
        return [m for m in sys.modules if m.startswith("milc") or m.startswith("qmk")]

    while len(mods()) > 0 and cycles < 25:
        for m in mods():
            try:
                del sys.modules[m]
            except:  # noqa: E722
                pass
        cycles += 1


def run_command(args) -> None:
    try:
        with patch.object(sys, "argv", args):
            import milc

            milc.cli.milc_options()
            import qmk_cli.subcommands  # noqa: F401
            import qmk.cli  # noqa: F401

            rc = milc.cli()
            if rc is False:
                sys.exit(1)
            elif rc is not True and isinstance(rc, int):
                if rc < 0 or rc > 255:
                    sys.exit(1)
                sys.exit(rc)

    finally:
        unload_qmk_cli()


for line in fileinput.input():
    run_command(shlex.split(line.strip()))
