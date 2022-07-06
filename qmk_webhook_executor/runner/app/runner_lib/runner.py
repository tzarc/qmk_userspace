# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later
import os
import subprocess
from rq import Queue
from redis import Redis

r = Redis(host='redis')
results_queue = Queue('results', connection=r)


def execute_run(target_repo: str, target_branch: str, pr_num: int, sha1: str, author: str, title: str):

    # Swap to the QMK directory
    os.chdir('/__w')

    # Run the script
    command = f'/app/build.sh --target-repo {target_repo} --target-branch {target_branch} --pr {pr_num} --sha1 {sha1} --author "{author}" --title "{title}"'
    print(command)
    subprocess.call(command, shell=True)

    if os.path.exists(f'/__w/{sha1}.html'):
        with open(f'/__w/{sha1}.html', 'r') as f:
            results = f.readlines()
            results_queue.enqueue(result_uploader, ttl=(86400*3), kwargs={
                'sha1': sha1,
                'results': '\n'.join(results)
            })
    else:
        results_queue.enqueue(result_uploader, ttl=(86400*3), kwargs={
            'sha1': sha1,
            'results': '<html><body>No build output was generated</body></html>'
        })


def result_uploader(sha1: str, results: str):
    pass
