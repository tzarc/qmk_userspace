#!/bin/bash
# Copyright 2018-2025 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

exitcode=0

max_search_lines=4

this_script=$(realpath "${BASH_SOURCE[0]}")
script_dir=$(dirname "$this_script")

license_check() {
    cat - | grep '\(Makefile.*\|Dockerfile.*\|\.\(c\|cpp\|h\|hpp\|mk\|sh\|py\|cmd\)\)$' | grep -v 'lib/lua' | grep -v 'board\.\(c\|h\|mk\)$' | grep -v '\(ch\|hal\|mcu\)conf\.h$' | grep -v '\.\(qgf\|qff\)\.\(c\|h\)$' | while read file; do
        if [[ -e $file ]]; then
            if [[ -z "$(cat "$file" | head -n${max_search_lines} | grep -E 'Copyright\s+[-0-9]+')" ]]; then
                echo "Missing license header: $(realpath $file)"
                exitcode=1
            elif [[ -z "$(cat "$file" | head -n${max_search_lines} | grep "SPDX-License-Identifier")" ]]; then
                echo "Missing license identifier: $(realpath $file)"
                exitcode=1
            fi
        fi
    done
}

cd $script_dir/..
git ls-files | license_check
update_license_headers.py

cd $script_dir/../modules/tzarc
git ls-files | license_check
update_license_headers.py

exit $exitcode
