#!/usr/bin/env python3
# Copyright 2018-2023 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

import datetime
import os
import shlex
import subprocess
import re
import sys
from pathlib import Path

if len(sys.argv) == 1:
    print("Usage: update_license_headers.py <ref-to-compare-against>")
    sys.exit(-1)

target_ref = sys.argv[1]

this_year = datetime.date.today().year

spdx_line = re.compile(
    r"Copyright (?P<startyear>\d+).*tzarc.*$", re.IGNORECASE | re.MULTILINE
)


def _run(command, capture_output=True, combined_output=False, text=True, **kwargs):
    if isinstance(command, str):
        command = shlex.split(command)
    if capture_output:
        kwargs["stdout"] = subprocess.PIPE
        kwargs["stderr"] = subprocess.PIPE
    if combined_output:
        kwargs["stderr"] = subprocess.STDOUT
    if "stdin" in kwargs and kwargs["stdin"] is None:
        del kwargs["stdin"]
    if text:
        kwargs["universal_newlines"] = True
    return subprocess.run(command, **kwargs)


diff_list = _run(f"git diff --name-only {target_ref}").stdout.strip().split("\n")

blacklist = [
    'check-license.sh',
    'update_license_headers.py',
    'generate_rgb_effects.py'
]

for diff_entry in [Path(p) for p in diff_list]:
    if diff_entry.is_file() and diff_entry.name not in blacklist:
        print(diff_entry.absolute())
        text = diff_entry.read_text()

        m = re.search(spdx_line, text)
        if m:
            start_year = int(m.group("startyear"))
            replacement = f"Copyright {start_year}-{this_year} Nick Brassel (@tzarc)"
            if start_year == this_year:
                replacement = f"Copyright {this_year} Nick Brassel (@tzarc)"
            new_text = re.sub(spdx_line, replacement, text)
            diff_entry.write_text(new_text, encoding="utf-8")
