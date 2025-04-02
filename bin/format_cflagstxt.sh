#!/bin/bash

format_cflagstxt() {
input_file="$1"

if [ ! -f "$input_file" ]; then
    echo "Error: File '$input_file' not found."
    exit 1
fi

# Read the file and split arguments into separate lines, respecting quoted strings
while IFS= read -r line; do
    echo "$line" | awk '
    {
        # Use a regular expression to split by spaces, respecting quoted strings
        while (match($0, /(-[^ ]*="[^"]*"|[^ ]+)/)) {
            token = substr($0, RSTART, RLENGTH)
            $0 = substr($0, RSTART + RLENGTH)

            # Check if the token is "-include"
            if (token == "-include") {
                # Match the next token and append it to "-include"
                if (match($0, /"([^"]+)"|[^ ]+/)) {
                    token = token " " substr($0, RSTART, RLENGTH)
                    $0 = substr($0, RSTART + RLENGTH)
                }
            }

            print token
        }
    }'
done < "$input_file"
}

if [ "$#" -lt 1 ]; then
    qmk_firmware=$(qmk config user.qmk_home | cut -d= -f2)
    find "$qmk_firmware/.build/" -name 'cflags.txt' -print0 | while IFS= read -r -d '' file; do
        format_cflagstxt "$file" > "${file}.tmp"
        mv "${file}.tmp" "$file"
    done
else
    while [ -n "$1" ]; do
        format_cflagstxt "$1"
        shift
    done
fi
