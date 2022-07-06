# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later
import os
from fastapi import FastAPI, Request
from fastapi.responses import JSONResponse
from rq import Queue
from redis import Redis
from runner_lib.runner import execute_run

r = Redis(host='redis')
jobs_queue = Queue('jobs', connection=r)
app = FastAPI(docs_url='/docs', redoc_url='/redoc')

expected_target_repo = os.getenv('TARGET_REPO')
ignored_prs = os.getenv('IGNORED_PRS')
if ignored_prs is None or ignored_prs == '':
    ignored_prs = []
else:
    ignored_prs = [int(s.strip()) for s in ignored_prs.split(',')]


@app.post("/qmk-webhook", include_in_schema=False)
async def qmk_webhook(request: Request):

    if request.headers.get('X-GitHub-Event') == 'ping':
        return {}

    elif request.headers.get('X-GitHub-Event') != 'pull_request':
        return JSONResponse(content={}, status_code=403)

    # Grab the json payload from the webhook
    json_blob = await request.json()

    # Make sure we're only acting on the repository we expect
    if json_blob['pull_request']['base']['repo']['full_name'] != expected_target_repo:
        return JSONResponse(content={}, status_code=403)

    # Set up the arguments we want to use
    invoke_args = {
        'target_repo': json_blob['pull_request']['base']['repo']['full_name'],
        'target_branch': json_blob['pull_request']['base']['ref'],
        'pr_num': json_blob['number'],
        'sha1': json_blob['pull_request']['head']['sha'],
        'author': json_blob['pull_request']['user']['login'],
        'title': json_blob['pull_request']['title']
    }

    ignored = ''
    if json_blob['number'] in ignored_prs:
        ignored = ', ignored'

    print(
        f'PR #{invoke_args["pr_num"]} ({json_blob["action"]}{ignored}): {invoke_args["title"]}')

    supported_operations = ['opened', 'synchronize']
    if json_blob['action'] not in supported_operations:
        return {}

    if json_blob['number'] in ignored_prs:
        return {}

    # Queue the work unit and exit
    jobs_queue.enqueue(execute_run, ttl=(86400*3), kwargs=invoke_args)
    return {}
