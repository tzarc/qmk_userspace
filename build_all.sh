#!/bin/bash

this_script=$(readlink -f "${BASH_SOURCE[@]}")
script_dir=$(dirname "$this_script")

MAKEFLAGS="-j --output-sync"

pushd "$script_dir/qmk_firmware" >/dev/null 2>&1 \
    && time make $MAKEFLAGS all:default MAKEFLAGS="$MAKEFLAGS" 2>&1 | tee "$script_dir/build.log" | egrep '\[(ERRORS|WARNINGS|OK)\]' \
    && popd >/dev/null 2>&1

num_successes=$(cat "$script_dir/build.log" | egrep '\[(OK)\]' | wc -l)
num_warnings=$(cat "$script_dir/build.log" | egrep '\[(WARNINGS)\]' | wc -l)
num_failures=$(cat "$script_dir/build.log" | egrep '\[(ERRORS)\]' | wc -l)
echo "-------------------------------"
echo "Successful builds: $num_successes"
echo "Warning builds: $num_warnings"
echo "Failing builds: $num_failures"
echo "-------------------------------"
cat "$script_dir/build.log" | egrep '\[(ERRORS)\]'