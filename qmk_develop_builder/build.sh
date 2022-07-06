#!/bin/bash
# Copyright 2018-2022 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-3.0-or-later

docker build -t qmk_develop_build_runner:latest .

# Create the container:
# docker create --name qmk_develop_runner -v #path-to#/.repo-hash:/home/qmk/.repo-hash -e 'AWS_KEY=AAAAAAAAAAAAAAAA' -e 'AWS_SECRET=@!(#*&!@(#*&!@(#*&!@(*#&!@(*&#' -e 'AWS_BUCKET=qmk.tzarc.io' -e 'DISCORD_WEBHOOK=https://discord.com/api/webhooks/aaaaaaaaaaaaaaaaaaa/bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb' qmk_develop_build_runner:latest

# Setup crontab:
# */15 * * * * [ -n "$(docker ps | grep qmk_develop_runner)" ] || docker start qmk_develop_runner >/dev/null 2>&1