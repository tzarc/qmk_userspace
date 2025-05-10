#!/usr/bin/env python3
import os
import sys
from pathlib import Path

qmk_firmware_path = Path(__file__).parent / Path("..") / "qmk_firmware"
qmk_cli_path = qmk_firmware_path / "lib/python"

sys.path.append(str(qmk_cli_path))

from qmk.keymap import locate_keymap  # noqa: E402
from qmk.path import (  # noqa: E402
    is_under_qmk_userspace,
    under_qmk_userspace,
    is_under_qmk_firmware,
    under_qmk_firmware,
)

orig_cwd = os.getcwd()
os.environ["ORIG_CWD"] = str(orig_cwd)
os.chdir(qmk_firmware_path)
os.environ["QMK_HOME"] = str(qmk_firmware_path)

for kbkm in sys.argv:
    try:
        (kb, km) = kbkm.split(":", 2)
        keymap_dir = Path(locate_keymap(kb, km)).parent
        if is_under_qmk_firmware(keymap_dir):
            print(under_qmk_firmware(keymap_dir))
        elif is_under_qmk_userspace(keymap_dir):
            print(under_qmk_userspace(keymap_dir))
    except:  # noqa: E722
        pass
