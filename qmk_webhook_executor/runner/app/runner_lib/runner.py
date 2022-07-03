import logging
import os
import subprocess

logger = logging.getLogger("runner")

def execute_run(pr_num: int, target_branch: str):

    # Swap to the QMK directory
    os.chdir('/qmk_firmware')

    # Run the script
    res = subprocess.call(f'/app/build.sh --target-branch {target_branch} --pr {pr_num}', shell=True)
    return res