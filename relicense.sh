#!/bin/bash
git grep '\(#\|//\) Copyright.*tzarc' | cut -d: -f1 | while read file ; do
    echo $file
    sed -i 's%^\(#\|//\) Copyright.*tzarc.*$%\1 Copyright 2018-2022 Nick Brassel (@tzarc)%g' $file
    sed -i 's%^\(#\|//\) SPDX-License-Identifier.*$%\1 SPDX-License-Identifier: GPL-2.0-or-later%g' $file
done
