#!/bin/bash
# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

base_rev=$1
target_rev=$2

added_files() {
    {
        diff --suppress-common-lines <(git ls-tree -r --name-only $base_rev) <(git ls-tree -r --name-only $target_rev) | awk '/^[>]/ {print $2}'
        git diff --name-status $base_rev $target_rev | awk '/^[R]/ {print $4}'
    } | sort | uniq
}

removed_files() {
    {
        diff --suppress-common-lines <(git ls-tree -r --name-only $base_rev) <(git ls-tree -r --name-only $target_rev) | awk '/^[<]/ {print $2}'
        git diff --name-status $base_rev $target_rev | awk '/^[R]/ {print $2}'
    } | sort | uniq
}

modified_files() {
    git diff --name-status $base_rev $target_rev | awk '/^[M]/ {print $2}'
}

echo -e "\e[38;5;10mAdded files:\e[0m"
added_files
echo -e "\e[38;5;9mRemoved files:\e[0m"
removed_files
echo -e "\e[38;5;11mModified files:\e[0m"
modified_files
