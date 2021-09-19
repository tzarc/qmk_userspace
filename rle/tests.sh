#!/usr/bin/env bash

set -eEuo pipefail

this_script="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(realpath "$(dirname "$this_script")")"

[[ -d "$script_dir/build" ]] || mkdir -p "$script_dir/build"
cd "$script_dir/build"

cmake -G Ninja ..
ninja clean
ninja

run_test() {
    local quiet=
    [ "${1:-}" == "-q" ] && quiet=1
    cat > tmp.orig

    ./qmk_rle -i tmp.orig -o tmp.rle -l
    ./qmk_rle -d -i tmp.rle -o tmp.dec

    local orig_sha1=$(sha1sum tmp.orig | awk '{print $1}')
    local dec_sha1=$(sha1sum tmp.dec | awk '{print $1}')

    if [[ -z $quiet ]] || [[ "$orig_sha1" != "$dec_sha1" ]]; then
        echo "Encoded:"
        cat tmp.rle | hexdump -vC
        echo "Decoded:"
        cat tmp.dec | hexdump -vC
    fi

    local RC=0
    if [[ "$orig_sha1" == "$dec_sha1" ]] ; then
        [ -z $quiet ] && echo -e "\e[1;32mFile sizes, RLE: $(stat --printf="%s" tmp.rle), Decoded: $(stat --printf="%s" tmp.dec)\e[0m"
    else
        echo -e "\e[1;31mFile sizes, RLE: $(stat --printf="%s" tmp.rle), Decoded: $(stat --printf="%s" tmp.dec)\e[0m"
        echo -e "\e[1;31mSHA1 mismatch: $orig_sha1 != $dec_sha1\e[0m"
        RC=1
    fi

    rm tmp.orig tmp.rle tmp.dec
    return $RC
}

{
    printf "0123456789abcdef"
    for n in {1..2}; do printf "a"; done
    printf "bbcdeeee"
} | run_test

{
    printf "0123456789abcdef"
    for n in {1..3}; do printf "a"; done
    printf "bbcdeeee"
} | run_test

{
    printf "0123456789abcdef"
    for n in {1..127}; do printf "a"; done
    printf "bbcdeeee"
} | run_test

{
    printf "0123456789abcdef"
    for n in {1..128}; do printf "a"; done
    printf "bbcdeeee"
} | run_test

{
    printf "0123456789abcdef"
    for n in {1..129}; do printf "a"; done
    printf "bbcdeeee"
} | run_test

{
    printf "0123456789abcdef"
    for n in {1..130}; do printf "a"; done
    printf "bbcdeeee"
} | run_test

{
    for n in {1..10}; do printf "0123456789abcdef"; done
} | run_test
