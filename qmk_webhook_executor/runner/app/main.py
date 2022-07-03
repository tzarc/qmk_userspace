import os
import logging
from fastapi import FastAPI, Request
from fastapi.responses import JSONResponse
from rq import Queue
from redis import Redis
from runner_lib.runner import execute_run

expected_target_repo = os.getenv('TARGET_REPO')
logger = logging.getLogger("acceptor")
r = Redis(host='redis')
q = Queue(connection=r)
app = FastAPI(docs_url='/docs', redoc_url='/redoc')


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

    if json_blob['action'] != 'synchronize':
        return {}

    # Set up the arguments we want to use
    invoke_args = {
        'target_repo': json_blob['pull_request']['base']['repo']['clone_url'],
        'target_branch': json_blob['pull_request']['base']['ref'],
        'pr_num': json_blob['number'],
        'sha1': json_blob['pull_request']['head']['sha'],
        'author': json_blob['pull_request']['user']['login'],
        'title': json_blob['pull_request']['title']
    }

    # Queue the work unit and exit
    q.enqueue(execute_run, ttl=(86400*3), kwargs=invoke_args)
    return {}

