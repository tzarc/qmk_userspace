#!/bin/bash
# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

exitcode=0

max_search_lines=4

git ls-files | grep '\(Makefile.*\|Dockerfile.*\|\.\(c\|cpp\|h\|hpp\|mk\|sh\|py\|cmd\)\)$' | grep -v 'board\.\(c\|h\|mk\)$' | grep -v '\(ch\|hal\|mcu\)conf\.h$' | grep -v '\.\(qgf\|qff\)\.\(c\|h\)$' | while read file ; do
    if [[ -e "$file" ]] ; then
        if [[ -z "$(cat "$file" | head -n${max_search_lines} | grep -E 'Copyright\s+[-0-9]+')" ]] ; then
            echo "Missing license header: $file"
            exitcode=1
        elif [[ -z "$(cat "$file" | head -n${max_search_lines} | grep "SPDX-License-Identifier")" ]] ; then
            echo "Missing license identifier: $file"
            exitcode=1
        fi
    fi
done

git grep '\(#\|//\|::\) Copyright.*tzarc' | cut -d: -f1 | while read file ; do
    sed -i 's%^\(#\|//\|::\) Copyright.*tzarc.*$%\1 Copyright 2018-'$(date +%Y)' Nick Brassel (@tzarc)%g' $file
    sed -i 's%^\(#\|//\|::\) SPDX-License-Identifier.*$%\1 SPDX-License-Identifier: GPL-2.0-or-later%g' $file
done

exit $exitcode
