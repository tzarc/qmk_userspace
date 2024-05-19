#!/usr/bin/env python3

import os
from pathlib import Path

qmk_firmware_dir = Path(os.path.realpath(__file__)).parent.parent / 'qmk_firmware'
docs_dir = qmk_firmware_dir / 'docs'

for mdfile in list(sorted(docs_dir.glob('**/*.md'))):
    if mdfile.parent.stem == 'ja' or mdfile.parent.stem == 'zh-cn':
        continue

    for line in mdfile.readlines():
        pass
