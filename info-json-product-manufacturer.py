#!/usr/bin/env python3
# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

import logging
import sys
import os
from milc import cli
from pathlib import Path

logging.basicConfig(level=logging.INFO)

def _set_log_level(level):
    cli.acquire_lock()
    old = cli.log_level
    cli.log_level = level
    cli.log.setLevel(level)
    logging.root.setLevel(level)
    cli.release_lock()
    return old

def _import_qmk_cli(qmk_firmware: Path):
    # Import the QMK CLI
    import qmk_cli.helpers
    if not qmk_cli.helpers.is_qmk_firmware(qmk_firmware):
        raise ImportError(f'Failed to detect repository')
    import qmk_cli.subcommands
    lib_python = str(qmk_firmware / 'lib/python')
    if lib_python not in sys.path:
        sys.path.append(lib_python)
    import qmk.cli  # also throws ImportError if problematic

qmk_firmware = Path('qmk_firmware').resolve()
os.chdir(qmk_firmware)
os.environ['QMK_HOME'] = str(qmk_firmware)
_import_qmk_cli(qmk_firmware)

import qmk.keyboard
import qmk.keymap
from qmk.info import info_json

keyboards = set(qmk.keyboard.list_keyboards())

for kb in sorted(keyboards):
    old = _set_log_level(logging.CRITICAL)
    data = info_json(kb)
    _set_log_level(old)
    if data['keyboard_name'].lower().startswith(f"{(data['manufacturer'])}".lower()):
        print(f"{kb:<40}: manufacturer={data['manufacturer']:<32}", f"keyboard_name={data['keyboard_name']}")
