/* Copyright 2021 Nick Brassel (@tzarc)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>

#define RLE_ENCODER
#define RLE_HAS_FILE_IO

#include "../qmk_firmware/quantum/rle.c"

int main(int argc, const char *argv[]) {
    bool        decode          = false;
    const char *input_filename  = "-";
    const char *output_filename = "-";
    for (int i = 1; i < argc; ++i) {
        const char *const arg = argv[i];
        if (strcmp(arg, "-d") == 0)  // decode
        {
            decode = true;
        } else if (strcmp(arg, "-i") == 0)  // input file
        {
            ++i;
            if (i >= argc) return -1;
            input_filename = argv[i];
        } else if (strcmp(arg, "-o") == 0)  // output file
        {
            ++i;
            if (i >= argc) return -1;
            output_filename = argv[i];
        }
    }

    FILE *input_file = strcmp(input_filename, "-") == 0 ? stdin : fopen(input_filename, "rb");
    if (!input_file) return -1;
    FILE *output_file = strcmp(output_filename, "-") == 0 ? stdout : fopen(output_filename, "wb");
    if (!output_file) return -1;

    file_rle_stream_t in  = make_file_rle_stream_t(input_file);
    file_rle_stream_t out = make_file_rle_stream_t(output_file);

    if (decode)
        rle_decode((rle_stream_t *)&in, (rle_stream_t *)&out);
    else
        rle_encode((rle_stream_t *)&in, (rle_stream_t *)&out);

    fclose(input_file);
    fclose(output_file);

    return 0;
}
