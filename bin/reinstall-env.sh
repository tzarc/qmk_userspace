#!/usr/bin/env bash
curl -fsSL https://install.qmk.fm/ | CONFIRM=1 SKIP_PACKAGE_MANAGER=1 SKIP_UV=1 SKIP_QMK_CLI=1 sh

post-checkout.sh
