# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later
from io import BytesIO
import boto3
import os
import subprocess
from rq import Queue
from redis import Redis
from discord_webhook import DiscordWebhook, DiscordEmbed

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
                'target_repo': target_repo,
                'target_branch': target_branch,
                'pr_num': pr_num,
                'sha1': sha1,
                'author': author,
                'title': title,
                'results': '\n'.join(results)
            })
    else:
        results_queue.enqueue(result_uploader, ttl=(86400*3), kwargs={
            'target_repo': target_repo,
            'target_branch': target_branch,
            'pr_num': pr_num,
            'sha1': sha1,
            'author': author,
            'title': title,
            'results': '<html><body>No build output was generated</body></html>'
        })


def result_uploader(target_repo: str, target_branch: str, pr_num: int, sha1: str, author: str, title: str, results: str):
    s3 = boto3.client('s3',
                      aws_access_key_id=os.getenv('AWS_ACCESS_KEY'),
                      aws_secret_access_key=os.getenv('AWS_SECRET'))

    s3.upload_fileobj(BytesIO(bytes(results, 'utf-8')), os.getenv('S3_BUCKET'),
                      sha1, ExtraArgs={'ContentType': 'text/html'})

    print()

    webhook = DiscordWebhook(url=os.getenv('DISCORD_WEBHOOK'), username="QMK GitHub CI")
    embed = DiscordEmbed(title=f'PR #{pr_num}: {title}', description=f'[#{pr_num} GitHub](https://github.com/qmk/qmk_firmware/pull/{pr_num}) / [#{pr_num} CI Results](https://qmk-ci.tzarc.io/{sha1})', color='00ff00')
    embed.add_embed_field(name='Author', value=author)
    embed.add_embed_field(name='Target Branch', value=target_branch)
    embed.add_embed_field(name='SHA1', value=sha1)
    embed.set_timestamp()
    webhook.add_embed(embed)
    webhook.execute()
    pass
