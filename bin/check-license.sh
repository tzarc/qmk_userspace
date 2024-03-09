#!/bin/bash
# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

exitcode=0

max_search_lines=4

this_script=$(realpath "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")
cd $script_dir/..

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

update_license_headers.py

exit $exitcode
