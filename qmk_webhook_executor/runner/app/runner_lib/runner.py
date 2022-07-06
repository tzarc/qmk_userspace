# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later
import logging
import os
import subprocess

logger = logging.getLogger("runner")


def execute_run(target_repo: str, target_branch: str, pr_num: int, sha1: str, author: str, title: str):

    # Swap to the QMK directory
    os.chdir('/__w')

    # Run the script
    res = subprocess.call(
        f'/app/build.sh --target-repo {target_repo} --target-branch {target_branch} --pr {pr_num} --sha1 {sha1}', shell=True)
    return res
