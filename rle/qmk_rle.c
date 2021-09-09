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

#define LZJB_COMPRESSOR

#include "../qmk_firmware/quantum/painter/rle.c"
#include "lzjb.c"

int main(int argc, const char* argv[]) {
    bool        decode          = false;
    const char* input_filename  = "-";
    const char* output_filename = "-";
    for (int i = 1; i < argc; ++i) {
        const char* const arg = argv[i];
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

    FILE* input_file = strcmp(input_filename, "-") == 0 ? stdin : fopen(input_filename, "rb");
    if (!input_file) return -1;
    FILE* output_file = strcmp(output_filename, "-") == 0 ? stdout : fopen(output_filename, "wb");
    if (!output_file) return -1;

    file_rle_stream_t in  = make_file_rle_stream_t(input_file);
    file_rle_stream_t out = make_file_rle_stream_t(output_file);

    int ret;
    if (decode) {
        ret = rle_decode((rle_stream_t*)&in, (rle_stream_t*)&out) ? 0 : -1;
    } else {
        ret = rle_encode((rle_stream_t*)&in, (rle_stream_t*)&out) ? 0 : -1;
    }

    long rle_size = 0;
    if (input_filename[0] != '-' && input_filename[1] != '\0') {
        rle_size = ftell(output_file);
    }

    fclose(input_file);
    fclose(output_file);

    if (input_filename[0] != '-' && input_filename[1] != '\0') {
        input_file = fopen(input_filename, "rb");
        if (input_file) {
            fseek(input_file, 0, SEEK_END);
            long length = ftell(input_file);
            fseek(input_file, 0, SEEK_SET);
            uint8_t* uncomp     = (uint8_t*)calloc(sizeof(uint8_t), length);
            uint8_t* comp       = (uint8_t*)calloc(sizeof(uint8_t), length * 2);
            size_t   compressed = lzjb_compress(uncomp, comp, length, length * 2);
            printf("         Original size: %d\n", (int)length);
            printf("   RLE-compressed size: %d\n", (int)rle_size);
            printf("  LZJB-compressed size: %d\n", (int)compressed);
            memset(uncomp, 0, length);
            uint8_t* uncomp2      = (uint8_t*)calloc(sizeof(uint8_t), length * 2);
            int      decompressed = lzjb_decompress(comp, uncomp2, compressed, length * 2);
            printf("LZJB-decompressed size: %d\n", (int)decompressed);
            printf("       LZJB comparison: %s\n", (memcmp(uncomp, uncomp2, length) == 0) ? "match" : "mismatch");
            free(uncomp2);
            free(comp);
            free(uncomp);
            fclose(input_file);
        }
    }

    return ret;
}
