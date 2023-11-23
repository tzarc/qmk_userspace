// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdio.h>
#include <string.h>
#include "keycodes.h"
#include "rle.h"

int main(int argc, const char* argv[]) {
    bool        help        = false;
    bool        decode      = false;
    const char* input_file  = NULL;
    const char* output_file = NULL;
    for (int i = 0; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'h':
                    help = true;
                    break;
                case 'd':
                    decode = true;
                    break;
                case 'i':
                    if (i + 1 >= argc) {
                        fprintf(stderr, "No input file specified");
                        return 1;
                    }
                    input_file = argv[++i];
                    break;
                case 'o':
                    if (i + 1 >= argc) {
                        fprintf(stderr, "No output file specified");
                        return 1;
                    }
                    output_file = argv[++i];
                    break;
            }
        }
    }

    if (help) {
        fprintf(stderr, "Usage: %s [-h] [-d] [-i input_file] [-o output_file]\n", argv[0]);
        fprintf(stderr, "  -h: Print this help message\n");
        fprintf(stderr, "  -d: Decode instead of encode\n");
        fprintf(stderr, "  -i: Specify input file (default: stdin)\n");
        fprintf(stderr, "  -o: Specify output file (default: stdout)\n");
        return 0;
    }

    FILE* input  = (input_file && strcmp(input_file, "-") != 0) ? fopen(input_file, "rb") : stdin;
    FILE* output = (output_file && strcmp(output_file, "-") != 0) ? fopen(output_file, "wb") : stdout;

    file_stream_t in_stream  = make_file_stream(input);
    file_stream_t out_stream = make_file_stream(output);

    if (decode) {
        rle_decode((stream_t*)&in_stream, (stream_t*)&out_stream);
    } else {
        rle_encode((stream_t*)&in_stream, (stream_t*)&out_stream);
    }

    return 0;
}
