/* Copyright 2020 QMK
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

/*
 * This file was auto-generated by util/convert_gfx.py!
 */

/* generated from lock-scrl.png */

#include <stdint.h>
#include <qp.h>
#include <qp_internal.h>

#if (QUANTUM_PAINTER_COMPRESSED_CHUNK_SIZE < 4096)
#    error Need to "#define QUANTUM_PAINTER_COMPRESSED_CHUNK_SIZE 4096" or greater in your config.h
#endif

// clang-format off

const uint32_t gfx_lock_scrl_chunk_offsets[1] = {
       0,  // chunk 0      // compressed size:  148 /   3.61%
};

static const uint8_t gfx_lock_scrl_chunk_data[148] = {
  0x01, 0x00, 0x00, 0xA0, 0x00, 0x09, 0x40, 0xFE, 0xFF, 0xFF, 0xBF, 0x01, 0x00, 0x00, 0xF4, 0xFF, 0x20, 0x00, 0x02, 0x1F, 0x00, 0x00, 0x20, 0x0E, 0x20, 0x10, 0x01, 0x00, 0x80, 0x20, 0x05, 0x20,
  0x00, 0x01, 0x02, 0xD0, 0x20, 0x04, 0x20, 0x00, 0x01, 0x07, 0xF0, 0x20, 0x04, 0x20, 0x00, 0x00, 0x0F, 0x60, 0x26, 0x20, 0x28, 0x03, 0xF8, 0xFF, 0xFF, 0x7F, 0x20, 0x0B, 0x01, 0x2F, 0xFC, 0x80,
  0x07, 0x00, 0x3F, 0xE0, 0x3A, 0x07, 0x02, 0x7E, 0xF4, 0xF9, 0x40, 0x47, 0x03, 0x7F, 0x78, 0x74, 0xF0, 0x60, 0x07, 0x02, 0x60, 0x14, 0xF8, 0x40, 0x07, 0x01, 0xFF, 0x02, 0x20, 0x93, 0x40, 0x5F,
  0x00, 0x0B, 0x20, 0x93, 0x18, 0x3F, 0xF8, 0xFF, 0x7F, 0x15, 0x50, 0xF5, 0xFF, 0x2F, 0xF4, 0xFF, 0x3F, 0x00, 0x00, 0xF0, 0xFF, 0x1F, 0xF0, 0xFF, 0x7F, 0x55, 0x55, 0xF5, 0xFF, 0x0F, 0xC0, 0xA7,
  0x20, 0x23, 0x40, 0x00, 0x40, 0x33, 0x20, 0x00, 0x00, 0xBF, 0xE0, 0x01, 0xD7, 0xC0, 0xE7, 0x60, 0x00, 0x01, 0x00, 0x00
};

const painter_compressed_image_descriptor_t gfx_lock_scrl_compressed = {
  .base = {
    .image_format = IMAGE_FORMAT_GRAYSCALE,
    .compression  = IMAGE_COMPRESSED_LZF,
    .width        = 32,
    .height       = 32
  },
  .image_bpp       = 2,
  .image_palette   = NULL,
  .chunk_count     = 1,
  .chunk_size      = 4096,
  .chunk_offsets   = gfx_lock_scrl_chunk_offsets,
  .compressed_data = gfx_lock_scrl_chunk_data,
  .compressed_size = 148  // original = 256 bytes (2bpp) /  57.81% of original // rgb24 = 3072 bytes /   4.82% of rgb24
};

painter_image_t gfx_lock_scrl = (painter_image_t)&gfx_lock_scrl_compressed;

// clang-format on
