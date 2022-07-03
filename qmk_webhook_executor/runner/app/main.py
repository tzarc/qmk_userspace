import logging
from fastapi import FastAPI
from fastapi.responses import HTMLResponse
from rq import Queue
from redis import Redis
import time

logger = logging.getLogger("acceptor")

from runner_lib.runner import execute_run

r = Redis(host='redis')
q = Queue(connection=r)

app = FastAPI(docs_url='/docs', redoc_url='/redoc')

@app.get("/", include_in_schema=False)
def base_page():
    job = q.enqueue(execute_run, kwargs={'pr_num':17534, 'target_branch':'develop'})

    for n in range(5000):
        logger.info(f'{n} -- {job.get_status()}')
        if job.get_status() == 'finished':
            break;
        time.sleep(0.1)

    return HTMLResponse(content=f'<html><body>{job.result}</body></html>', status_code=200)
