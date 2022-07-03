import logging

logger = logging.getLogger("runner")

def execute_run(pr_num):
    logger.info(pr_num)
    return pr_num