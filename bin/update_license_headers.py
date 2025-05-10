#!/usr/bin/env python3
# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

import datetime
import os
import shlex
import subprocess
import re
import sys
from pathlib import Path

max_search_lines = 4

target_ref = sys.argv[1] if len(sys.argv) > 1 else None

this_year = datetime.date.today().year

spdx_line = re.compile(r"Copyright (?P<startyear>\d+).*tzarc.*$", re.IGNORECASE | re.MULTILINE)


def _run(command, capture_output: bool=True, combined_output: bool=False, text: bool=True, **kwargs):
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


git_command = "git ls-files" if target_ref is None else f"git diff --name-only {target_ref}"
diff_list = _run(git_command).stdout.strip().split("\n")
diff_list.extend(_run(f"{git_command} --cached").stdout.strip().split("\n"))
diff_list = list(sorted(set(diff_list)))

source_files = [".sh", ".py", ".c", ".cxx", ".cpp", ".cc", ".h", ".hxx", ".hpp", ".hh", ".inl", ".mk"]

for diff_entry in [Path(p) for p in diff_list]:
    if diff_entry.is_file() and diff_entry.suffix in source_files:
        text = diff_entry.read_text().split("\n")
        header = "\n".join(text[:max_search_lines])
        rest = "\n".join(text[max_search_lines:])

        m = re.search(spdx_line, header)
        if m:
            start_year = int(m.group("startyear"))
            replacement = f"Copyright {start_year}-{this_year} Nick Brassel (@tzarc)"
            if start_year == this_year:
                replacement = f"Copyright {this_year} Nick Brassel (@tzarc)"
            new_header = re.sub(spdx_line, replacement, header)
            if new_header != header:
                print(f"Updating license header: {diff_entry}")
                diff_entry.write_text("\n".join([new_header, rest]), encoding="utf-8")
