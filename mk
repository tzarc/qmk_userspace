#!/bin/bash

havecmd()  { command command type "${1}" >/dev/null 2>&1 || return 1 ; }

havecmd make && MAKE_CMD=make
havecmd gmake && MAKE_CMD=gmake

make_target="${1:-bin}"
time $MAKE_CMD -j$(nproc) --output-sync $make_target 2>&1 \
    | egrep --line-buffered -iv '(Entering|Leaving) directory' \
    | egrep --line-buffered -iv 'Bad file descriptor'
