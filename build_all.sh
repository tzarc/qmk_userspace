#!/bin/bash

this_script=$(readlink -f "${BASH_SOURCE[@]}")
script_dir=$(dirname "$this_script")

export PATH=/home/nickb/gcc-arm/gcc-arm-none-eabi-8-2018-q4-major/bin:$PATH
MAKEFLAGS="-j --output-sync"

pushd "$script_dir/qmk_firmware" >/dev/null 2>&1 \
    && time remake $MAKEFLAGS all-chibios:default "$@" 2>&1 | tee "$script_dir/build.log" | egrep '\[(ERRORS|WARNINGS|OK)\]' \
    && popd >/dev/null 2>&1

#pushd "$script_dir/qmk_firmware" >/dev/null 2>&1 \
#    && time { ./util/list_keyboards.sh | sort | uniq | parallel --eta --bar --plain --group --keep-order --retries 3 --jobs 200% remake -j4 -O {}:default SILENT=true REQUIRE_PLATFORM_KEY=chibios ; } | tee "$script_dir/build.log" | egrep '\[(ERRORS|WARNINGS|OK)\]' \
#    && popd >/dev/null 2>&1

num_successes=$(cat "$script_dir/build.log" | egrep '\[(OK)\]' | wc -l)
num_skipped=$(cat "$script_dir/build.log" | egrep '\[(SKIPPED)\]' | wc -l)
num_warnings=$(cat "$script_dir/build.log" | egrep '\[(WARNINGS)\]' | wc -l)
num_failures=$(cat "$script_dir/build.log" | egrep '\[(ERRORS)\]' | wc -l)
echo "-------------------------------"
echo "Successful builds: $num_successes"
echo "Skipped builds: $num_skipped"
echo "Warning builds: $num_warnings"
echo "Failing builds: $num_failures"
echo "-------------------------------"
cat "$script_dir/build.log" | egrep '\[(ERRORS)\]'