/* Copyright 2021 QMK
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
 * This file was auto-generated by `qmk painter-convert-graphics -i djinn.png -f mono16 -c -s 4096`
 */

#include <progmem.h>
#include <stdint.h>
#include <qp.h>
#include <qp_internal.h>

#ifndef QUANTUM_PAINTER_COMPRESSION_ENABLE
#    error Compression is not available on your selected platform. Please regenerate djinn without compression.
#endif

#if (QUANTUM_PAINTER_COMPRESSED_CHUNK_SIZE < 4096)
#    error Need to "#define QUANTUM_PAINTER_COMPRESSED_CHUNK_SIZE 4096" or greater in your config.h
#endif

// clang-format off

static const uint32_t gfx_djinn_chunk_offsets[4] PROGMEM = {
         0,  // chunk   0 // compressed size:   1329 bytes / 32.45% of 4096 bytes
      1329,  // chunk   1 // compressed size:   1653 bytes / 40.36% of 4096 bytes
      2982,  // chunk   2 // compressed size:   1242 bytes / 30.32% of 4096 bytes
      4224,  // chunk   3 // compressed size:    535 bytes / 13.06% of 4096 bytes
};

static const uint8_t gfx_djinn_chunk_data[4759] PROGMEM = {
    0x01, 0x00, 0x00, 0xE0, 0x73, 0x00, 0x00, 0x22, 0xE0, 0x28, 0x7C, 0x02, 0x10, 0x96, 0x02, 0xE0, 0x27, 0x33, 0x02, 0x10, 0xE7, 0x28, 0xE0, 0x28, 0x32, 0x02, 0xF9, 0x7E, 0x01, 0xE0, 0x26, 0x33,
    0x03, 0x20, 0xFC, 0xDF, 0x05, 0xE0, 0x26, 0x32, 0x03, 0x30, 0xFE, 0xFF, 0x2B, 0xE0, 0x26, 0x32, 0x03, 0x90, 0xFF, 0xFF, 0x7F, 0xE0, 0x26, 0x99, 0x04, 0xE3, 0xFF, 0xFF, 0xEF, 0x03, 0xE0, 0x24,
    0x66, 0x05, 0x20, 0xFC, 0xFF, 0xFF, 0xFF, 0x08, 0xE0, 0x24, 0x32, 0x00, 0xA2, 0x20, 0x31, 0x02, 0xFF, 0x2D, 0x10, 0xE0, 0x22, 0x9A, 0x01, 0x30, 0xFA, 0x20, 0x31, 0x03, 0xFF, 0x5F, 0x30, 0x27,
    0xE0, 0x20, 0x67, 0x01, 0x10, 0xB5, 0x20, 0x30, 0x04, 0xFF, 0xFF, 0x9F, 0x30, 0x9C, 0xE0, 0x20, 0xCE, 0x00, 0x61, 0x40, 0xC9, 0x05, 0xFF, 0xFF, 0xDF, 0x20, 0xEB, 0x3A, 0xE0, 0x1E, 0x66, 0x01,
    0x10, 0xD7, 0x20, 0x2F, 0x20, 0x00, 0x03, 0xEF, 0x21, 0xFA, 0xBF, 0xE0, 0x1E, 0x66, 0x01, 0x81, 0xFE, 0x20, 0x2F, 0x40, 0x00, 0x03, 0x12, 0xF9, 0xFF, 0x3B, 0xE0, 0x1C, 0x66, 0x01, 0x10, 0xE9,
    0x40, 0x2E, 0x40, 0x00, 0x20, 0x32, 0x00, 0xBF, 0xE1, 0x1C, 0x34, 0x00, 0xA1, 0x40, 0x2D, 0x40, 0x00, 0x20, 0x98, 0x02, 0xFF, 0xFF, 0x1B, 0xE0, 0x1A, 0x66, 0x00, 0x20, 0x20, 0x27, 0x80, 0x00,
    0x05, 0xDF, 0x20, 0xFB, 0xFF, 0xFF, 0x8F, 0xE0, 0x1A, 0x66, 0x00, 0x92, 0x80, 0x2F, 0x20, 0x00, 0x02, 0x9F, 0x30, 0xFD, 0x20, 0x6A, 0x00, 0x06, 0xE0, 0x18, 0x66, 0x01, 0x20, 0xE8, 0x20, 0x2C,
    0x80, 0x00, 0x01, 0x5F, 0x50, 0x40, 0x07, 0x00, 0x4D, 0xE0, 0x18, 0x32, 0x00, 0x71, 0xC1, 0x2E, 0x03, 0xFF, 0xFF, 0x1C, 0x91, 0x20, 0x04, 0x01, 0xFF, 0xAF, 0xE3, 0x17, 0xD1, 0x01, 0x10, 0xD6,
    0x20, 0x25, 0x80, 0x00, 0x01, 0xEF, 0x06, 0x22, 0xCE, 0x20, 0x06, 0x00, 0x17, 0xE0, 0x16, 0x66, 0x00, 0x40, 0x81, 0xF8, 0x40, 0x00, 0x02, 0x8F, 0x01, 0xF9, 0x40, 0x06, 0x01, 0xFF, 0x4C, 0xE0,
    0x16, 0x32, 0x00, 0xB2, 0x20, 0x23, 0xA0, 0x00, 0x01, 0x29, 0x50, 0x80, 0xA3, 0x00, 0x9F, 0xE0, 0x15, 0x99, 0x01, 0x10, 0xF8, 0xA0, 0x2F, 0x04, 0xFF, 0xFF, 0x8E, 0x02, 0xD4, 0x20, 0x05, 0x20,
    0x00, 0x00, 0xEF, 0xE4, 0x15, 0x04, 0xA0, 0x58, 0x20, 0x00, 0x02, 0xCF, 0x06, 0x50, 0x21, 0x62, 0x60, 0x00, 0xE2, 0x15, 0x34, 0x00, 0xC2, 0x60, 0x23, 0x20, 0x00, 0x03, 0xEF, 0x3A, 0x00, 0xD5,
    0x20, 0x06, 0x60, 0x00, 0x00, 0x6F, 0xE1, 0x14, 0xCD, 0xE0, 0x00, 0x97, 0x03, 0x7C, 0x01, 0x00, 0xC5, 0x60, 0x2F, 0x20, 0x00, 0x00, 0xBF, 0xE0, 0x13, 0xCC, 0x24, 0x8A, 0x80, 0x00, 0x00, 0x9D,
    0x22, 0xF4, 0x01, 0x40, 0xD9, 0x80, 0x0B, 0x01, 0xFF, 0xFF, 0xE2, 0x13, 0x00, 0x24, 0x8A, 0x60, 0x00, 0x01, 0xBE, 0x16, 0x41, 0x58, 0x01, 0x51, 0xEA, 0x60, 0x0C, 0xE2, 0x15, 0x9A, 0x00, 0xD3,
    0x20, 0x1F, 0x20, 0x00, 0x01, 0xCF, 0x37, 0x40, 0x31, 0x02, 0x00, 0x00, 0x62, 0xA1, 0xA4, 0x00, 0x3E, 0x20, 0x0B, 0xE0, 0x0F, 0x00, 0x80, 0xCA, 0x01, 0xEF, 0x4A, 0xA0, 0xEF, 0x01, 0x10, 0x93,
    0x80, 0xA8, 0x00, 0x6F, 0xE0, 0x0F, 0x2F, 0x20, 0x22, 0x80, 0x55, 0x00, 0x6D, 0xE0, 0x00, 0xEE, 0xA2, 0x3E, 0x00, 0xAF, 0x20, 0x1B, 0xE0, 0x0E, 0x00, 0x81, 0x95, 0x00, 0xCF, 0x20, 0xFA, 0xC0,
    0x00, 0x82, 0x3F, 0x00, 0xDF, 0xC0, 0x0E, 0xE0, 0x09, 0x00, 0x60, 0xFD, 0x01, 0xEF, 0x19, 0xE0, 0x03, 0x18, 0x00, 0xC3, 0x20, 0xDA, 0x01, 0xFF, 0xFF, 0xA0, 0xA8, 0xE0, 0x0A, 0x00, 0x00, 0xB1,
    0x20, 0x1D, 0x00, 0xFF, 0xE6, 0x05, 0x57, 0x00, 0x40, 0x62, 0x02, 0xE0, 0x00, 0xA9, 0xE0, 0x08, 0x00, 0x42, 0x54, 0x00, 0xDF, 0x61, 0x5F, 0xE0, 0x01, 0x00, 0x00, 0xF6, 0x20, 0x45, 0x00, 0xFF,
    0xE0, 0x02, 0xAA, 0xE0, 0x06, 0x00, 0x43, 0x2B, 0x00, 0x6D, 0xE0, 0x06, 0x84, 0x60, 0x79, 0x00, 0x04, 0xE0, 0x06, 0x28, 0xE0, 0x01, 0x00, 0x00, 0xE7, 0x20, 0xC9, 0x00, 0x18, 0xE0, 0x01, 0x0E,
    0x60, 0x00, 0x00, 0x40, 0x41, 0x67, 0xE0, 0x11, 0x32, 0x21, 0x96, 0xE2, 0x08, 0x4F, 0x00, 0x10, 0x44, 0x70, 0xE0, 0x11, 0x32, 0x23, 0x85, 0x00, 0x6D, 0x60, 0x5A, 0xE0, 0x03, 0x00, 0x00, 0xE4,
    0xE0, 0x14, 0xCB, 0x20, 0x32, 0xE4, 0x09, 0xE9, 0x43, 0xA7, 0xE1, 0x11, 0x31, 0x03, 0xF9, 0xFF, 0xEF, 0x07, 0xE0, 0x03, 0x60, 0x60, 0x00, 0x00, 0x71, 0x20, 0x65, 0xE1, 0x06, 0x12, 0xE0, 0x02,
    0x00, 0x02, 0xF8, 0xFF, 0xDF, 0xE1, 0x09, 0x4E, 0x20, 0xFF, 0x00, 0xEF, 0xE0, 0x02, 0x23, 0xE0, 0x06, 0x00, 0x02, 0xE7, 0xFF, 0xBF, 0xE0, 0x06, 0x11, 0x20, 0x00, 0x25, 0x65, 0x00, 0xCF, 0x20,
    0x06, 0xE0, 0x0E, 0x00, 0x02, 0xD6, 0xFF, 0x9F, 0xE0, 0x09, 0x19, 0x24, 0x41, 0xE2, 0x12, 0x96, 0x02, 0xC4, 0xFF, 0x7E, 0xE0, 0x09, 0x32, 0x21, 0xFF, 0x00, 0x7F, 0x20, 0x05, 0xE0, 0x0E, 0x00,
    0x02, 0xB1, 0xFF, 0x6E, 0xE0, 0x0A, 0x19, 0x02, 0xD5, 0xFF, 0x4F, 0xE0, 0x0A, 0x15, 0xA0, 0x00, 0x02, 0x90, 0xFE, 0x5E, 0xA0, 0x09, 0xE0, 0x03, 0x00, 0x02, 0xC3, 0xFF, 0x2E, 0xE0, 0x03, 0x0E,
    0xE0, 0x05, 0x00, 0x02, 0x50, 0xFE, 0x4E, 0xE0, 0x05, 0x10, 0x60, 0x00, 0x00, 0xB3, 0xE1, 0x0A, 0xAC, 0xE0, 0x00, 0x00, 0x01, 0x10, 0xFC, 0xE0, 0x0B, 0x32, 0x01, 0xA2, 0xFF, 0xE2, 0x07, 0x77,
    0xE0, 0x03, 0x00, 0x00, 0xE7, 0xE0, 0x0B, 0x98, 0x00, 0x91, 0xE2, 0x12, 0x62, 0x02, 0x00, 0x00, 0xC1, 0xE0, 0x0B, 0xFE, 0x01, 0x81, 0xCF, 0xE2, 0x11, 0x2F, 0x02, 0x00, 0x00, 0x60, 0xE1, 0x0B,
    0x64, 0x00, 0x80, 0xE7, 0x14, 0x2E, 0x01, 0x10, 0x8B, 0x20, 0x37, 0x40, 0x00, 0x00, 0x41, 0x20, 0x04, 0x00, 0x21, 0xA2, 0x59, 0x00, 0x80, 0xE0, 0x0B, 0xAD, 0xE0, 0x01, 0x00, 0x00, 0x84, 0x60,
    0x0A, 0x02, 0x10, 0x52, 0xB8, 0x20, 0x07, 0x02, 0x81, 0x58, 0x12, 0x20, 0x05, 0x20, 0x32, 0x00, 0x2A, 0x20, 0x04, 0xE0, 0x11, 0x00, 0x40, 0x5D, 0x03, 0x00, 0x31, 0x85, 0xDA, 0x42, 0x6B, 0x03,
    0xA0, 0xDF, 0x8A, 0x35, 0x40, 0x68, 0x00, 0x61, 0xE6, 0x15, 0x5F, 0x40, 0x00, 0x04, 0x62, 0xB9, 0xFD, 0xFF, 0x8F, 0x20, 0x08, 0x04, 0x60, 0xFE, 0xFF, 0xBD, 0x69, 0x20, 0xF9, 0xC0, 0xA0, 0xE0,
    0x11, 0x00, 0x01, 0x83, 0xDC, 0x23, 0x20, 0x45, 0x85, 0x00, 0x20, 0x25, 0x3B, 0x01, 0xDF, 0x8C, 0x20, 0x33, 0xE0, 0x17, 0x00, 0x01, 0x82, 0xED, 0x20, 0x31, 0x87, 0x85, 0x00, 0xC4, 0x40, 0x09,
    0xE3, 0x09, 0xA6, 0xE0, 0x07, 0x00, 0x00, 0x20, 0x68, 0xF3, 0x00, 0x4B, 0x40, 0xC3, 0x2B, 0x91, 0x20, 0x33, 0x40, 0xA2, 0xE0, 0x15, 0x00, 0x00, 0x81, 0x20, 0x25, 0xE4, 0x00, 0xCB, 0x01, 0x30,
    0xEB, 0x20, 0x9A, 0x60, 0x3D, 0xE0, 0x13, 0x00, 0x45, 0x30, 0x00, 0xBE, 0xE4, 0x00, 0x83, 0x00, 0x82, 0x23, 0xC4, 0xE1, 0x17, 0x2F, 0x01, 0x10, 0xF7, 0x20, 0xC8, 0x00, 0x29, 0xE0, 0x00, 0x53,
    0x20, 0x9A, 0x00, 0xEF, 0xEA, 0x17, 0xFA, 0x23, 0xE8, 0xE8, 0x04, 0x1A, 0x2A, 0x00, 0x00, 0x0A, 0xE0, 0x00, 0x40, 0xE0, 0x0D, 0x00, 0x49, 0x4E, 0xEB, 0x03, 0xEB, 0x29, 0x9C, 0x00, 0x1C, 0xE0,
    0x0D, 0x29, 0xE0, 0x00, 0x00, 0x00, 0x40, 0x20, 0xEF, 0x20, 0xC9, 0x02, 0x10, 0x21, 0x33, 0x20, 0x00, 0x40, 0xF8, 0x02, 0xD4, 0xFF, 0x3D, 0xE0, 0x00, 0x1C, 0xE0, 0x0D, 0x00, 0x29, 0x81, 0x4C,
    0x1D, 0x05, 0x60, 0xCB, 0xCC, 0xCC, 0xCC, 0xAC, 0x41, 0xC5, 0x02, 0x91, 0xFF, 0x5D, 0xE0, 0x0D, 0x29, 0xE0, 0x00, 0x00, 0x04, 0x60, 0xFF, 0xFF, 0x3C, 0x10, 0x60, 0x8F, 0x02, 0xFF, 0xFF, 0x6E,
    0x40, 0x65, 0x01, 0x30, 0xFE, 0xE3, 0x0B, 0x0F, 0xE0, 0x03, 0x00, 0x25, 0x4D, 0x01, 0x17, 0xB2, 0x20, 0x2A, 0x46, 0x55, 0x49, 0x87, 0x03, 0x40, 0x02, 0xFA, 0x9E, 0xE0, 0x03, 0x1F, 0xE0, 0x0A,
    0x00, 0x00, 0x71, 0x27, 0x5C, 0x00, 0xE8, 0x25, 0x97, 0x01, 0x10, 0xFD, 0x89, 0xED, 0x03, 0xC4, 0x05, 0xF4, 0xBF, 0xEC, 0x16, 0x2C, 0x05, 0x81, 0xFF, 0x8F, 0x11, 0xFD, 0x2C, 0x20, 0x4B, 0x00,
    0xE4, 0x60, 0xC3, 0x06, 0x10, 0xF8, 0x19, 0xE1, 0xBF, 0x22, 0x36, 0x20, 0x0F, 0xE0, 0x11, 0x00, 0x05, 0x81, 0xFF, 0x5E, 0x60, 0xFE, 0x9E, 0x20, 0x99, 0x01, 0x90, 0xFF, 0x43, 0x58, 0x06, 0x50,
    0xFD, 0x3D, 0x90, 0xCF, 0x14, 0x79, 0x20, 0x0F, 0xE0, 0x11, 0x00, 0x03, 0x71, 0xFF, 0x3C, 0xA0, 0x67, 0x61, 0x01, 0x30, 0xED, 0x20, 0x9E, 0x01, 0x00, 0xB4, 0x2A, 0xC5, 0x02, 0xDF, 0x05, 0xD8,
    0xE2, 0x14, 0x33, 0x02, 0x71, 0xFF, 0x19, 0x29, 0x92, 0x04, 0x5C, 0x01, 0x00, 0x10, 0xC7, 0x21, 0x9E, 0x2C, 0xC6, 0x04, 0x8F, 0x21, 0xEE, 0x07, 0xE7, 0xEA, 0x14, 0x5D, 0x02, 0x60, 0xEF, 0x07,
    0x29, 0xB9, 0x00, 0xCF, 0x27, 0xC8, 0x00, 0x62, 0x20, 0x8D, 0x00, 0xB4, 0x27, 0x3B, 0x03, 0x11, 0xFC, 0x08, 0xD5, 0xEC, 0x13, 0x5F, 0x03, 0x10, 0x60, 0xDF, 0x05, 0x4B, 0x5A, 0x29, 0x8E, 0x21,
    0x9D, 0x00, 0x82, 0x28, 0x26, 0x04, 0xEF, 0x12, 0xFA, 0x1A, 0xC3, 0xEA, 0x13, 0x2A, 0x03, 0x55, 0x50, 0xCF, 0x04, 0x41, 0xC5, 0x06, 0xEF, 0xAD, 0x36, 0x12, 0x32, 0x95, 0xEC, 0x21, 0xCF, 0x06,
    0xFF, 0x13, 0xF7, 0x2B, 0xB2, 0xFF, 0xBF, 0x20, 0xA5, 0xE0, 0x0D, 0x00, 0x03, 0x50, 0x6D, 0x50, 0xAF, 0x02, 0x22, 0xFB, 0xFF, 0x40, 0x00, 0x03, 0xEE, 0xEE, 0xEE, 0xFE, 0x40, 0x07, 0x08, 0xFF,
    0x06, 0xF5, 0x4C, 0xA0, 0xFF, 0xFF, 0x5D, 0x00, 0xE0, 0x0D, 0x00, 0x06, 0x10, 0xE7, 0x6F, 0x50, 0x9F, 0x41, 0xFC, 0x20, 0x27, 0xE0, 0x02, 0x00, 0x07, 0x08, 0xF3, 0x5D, 0x90, 0xFE, 0xFF, 0xEF,
    0x39, 0xE0, 0x0C, 0x32, 0x07, 0x10, 0x94, 0xFE, 0x4F, 0x60, 0x8E, 0x60, 0xFD, 0xE0, 0x02, 0x2F, 0x20, 0x00, 0x03, 0x0B, 0xD3, 0x6E, 0x70, 0x20, 0x6F, 0x02, 0xCF, 0x26, 0x01, 0xE0, 0x09, 0x34,
    0x05, 0x20, 0x84, 0xFD, 0xFF, 0x3F, 0x81, 0x60, 0x1F, 0xE0, 0x03, 0x00, 0x03, 0x0D, 0xB2, 0x8F, 0x50, 0x40, 0x45, 0x02, 0xCF, 0x47, 0x12, 0xE0, 0x06, 0x33, 0x02, 0x10, 0x43, 0xA6, 0x20, 0x18,
    0x03, 0x1E, 0xA1, 0x5D, 0x80, 0xE0, 0x03, 0x2F, 0x20, 0x00, 0x03, 0x1E, 0x91, 0x9F, 0x31, 0x20, 0x19, 0x06, 0xFF, 0xFF, 0xDF, 0x7A, 0x45, 0x33, 0x23, 0xC0, 0x6A, 0x05, 0x20, 0x44, 0x55, 0x76,
    0x98, 0xDB, 0x20, 0x15, 0x04, 0xFF, 0x1B, 0xC1, 0x4C, 0x91, 0x20, 0x06, 0xE0, 0x03, 0x00, 0x03, 0x5F, 0x50, 0xAD, 0x12, 0xA0, 0xDE, 0x04, 0xDE, 0xBC, 0xBB, 0x69, 0x02, 0x60, 0x6A, 0x04, 0x10,
    0xA6, 0xCC, 0xDD, 0xEE, 0x80, 0x25, 0x03, 0x07, 0xE3, 0x2B, 0xB2, 0x80, 0x09, 0xE0, 0x00, 0x00, 0x03, 0xBF, 0x11, 0x86, 0x03, 0x81, 0x44, 0x40, 0x00, 0xA1, 0x06, 0x00, 0x71, 0x60, 0x77, 0x40,
    0x00, 0x04, 0xEF, 0x03, 0xE7, 0x07, 0xD4, 0x40, 0x08, 0xE0, 0x02, 0x00, 0x03, 0xEF, 0x17, 0x21, 0x11, 0xE0, 0x01, 0x32, 0x02, 0xFF, 0xAE, 0x03, 0x40, 0x66, 0x00, 0xD6, 0x20, 0x09, 0x80, 0x00,
    0x04, 0xAF, 0x02, 0x9C, 0x11, 0xF8, 0x80, 0x0A, 0xE0, 0x01, 0x00, 0x02, 0x9E, 0x03, 0x71, 0x21, 0x31, 0xC0, 0x00, 0x01, 0xEF, 0x29, 0x20, 0x32, 0x00, 0x30, 0xE0, 0x01, 0x43, 0x02, 0x9F, 0x01,
    0x13, 0x61, 0x1D, 0xE0, 0x05, 0x00, 0x01, 0xCE, 0xED, 0xE0, 0x03, 0x0F, 0x00, 0x7E, 0x20, 0x32, 0x00, 0x81, 0xE0, 0x01, 0x10, 0x03, 0xDF, 0x13, 0x11, 0xD5, 0xE0, 0x01, 0x0D, 0x80, 0x00, 0x04,
    0xEE, 0xBD, 0x79, 0x56, 0x96, 0xE0, 0x02, 0x34, 0x04, 0xCF, 0x04, 0x00, 0x00, 0xC3, 0x80, 0x1A, 0x60, 0x00, 0x03, 0x8D, 0xA7, 0xED, 0xDD, 0x80, 0x00, 0x82, 0x2F, 0x03, 0xEF, 0xDD, 0x9B, 0x36,
    0x41, 0x57, 0xE0, 0x02, 0xEE, 0x04, 0xEE, 0x17, 0x00, 0x00, 0xC4, 0xE0, 0x00, 0xA9, 0x1B, 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x55, 0x44, 0x34, 0x33, 0x44, 0x44, 0x55, 0x66, 0x87, 0x98, 0xAA, 0xBB,
    0xBB, 0x8A, 0x25, 0x00, 0x10, 0x32, 0x54, 0x66, 0x45, 0x11, 0xE6, 0x60, 0x51, 0x0A, 0xEF, 0xDD, 0xBC, 0x9A, 0x67, 0x13, 0x00, 0x00, 0x41, 0x97, 0xBA, 0x80, 0x77, 0x02, 0xCE, 0x79, 0x35, 0x20,
    0x51, 0x01, 0x10, 0x11, 0x40, 0x00, 0x20, 0x08, 0x03, 0x00, 0x00, 0x11, 0x22, 0x21, 0xE1, 0x06, 0x86, 0xCA, 0xFE, 0xFF, 0xDF, 0x17, 0x92, 0x41, 0x95, 0x02, 0x9A, 0x78, 0x45, 0x20, 0x2F, 0x20,
    0x1A, 0x04, 0x01, 0x00, 0x31, 0x65, 0x98, 0x20, 0x35, 0x08, 0xCF, 0x47, 0x01, 0x10, 0x43, 0x65, 0x77, 0x88, 0x99, 0x20, 0x00, 0x02, 0x89, 0x68, 0x24, 0x20, 0x32, 0x03, 0x00, 0x20, 0x64, 0x97,
    0x21, 0xE1, 0x20, 0x7C, 0x06, 0x29, 0x50, 0xAB, 0x78, 0x56, 0x34, 0x12, 0x40, 0x72, 0x06, 0x76, 0x48, 0x00, 0x10, 0x96, 0x57, 0x13, 0x80, 0x0A, 0x04, 0x01, 0x63, 0xA9, 0xCB, 0xED, 0x60, 0xA5,
    0x09, 0xCD, 0x8A, 0x56, 0x23, 0x00, 0x00, 0x31, 0x75, 0xA9, 0xDC, 0x20, 0x30, 0x05, 0x9B, 0x67, 0x34, 0x23, 0x11, 0x10, 0x22, 0x2A, 0x0C, 0x10, 0x43, 0x75, 0x98, 0xBA, 0xDC, 0xFE, 0x7F, 0x01,
    0x10, 0xFA, 0xDE, 0xBC, 0x60, 0x91, 0x02, 0x01, 0x82, 0xEC, 0x60, 0xB0, 0x02, 0xFF, 0xAD, 0x58, 0x20, 0x4F, 0x03, 0x20, 0x54, 0x97, 0xCB, 0x40, 0xAF, 0x01, 0xAD, 0x46, 0x20, 0x2C, 0x20, 0x00,
    0x04, 0x21, 0x64, 0x97, 0xBA, 0xDC, 0x40, 0x10, 0x21, 0x89, 0x01, 0x10, 0xFA, 0x20, 0x49, 0x05, 0xDD, 0xBC, 0x8A, 0x16, 0x20, 0xEB, 0x20, 0x0F, 0x08, 0xFF, 0xEF, 0x9C, 0x46, 0x22, 0x01, 0x00,
    0x41, 0x96, 0x80, 0x6F, 0x02, 0xDF, 0x59, 0x13, 0x20, 0x5D, 0x00, 0x42, 0x40, 0x5D, 0x80, 0x7F, 0x20, 0x00, 0x80, 0x32, 0x40, 0x00, 0x00, 0x19, 0x23, 0x7B, 0x08, 0xFF, 0xFF, 0x9D, 0x26, 0x11,
    0x00, 0x00, 0x63, 0xC9, 0xA0, 0x1F, 0x01, 0xAE, 0x25, 0x20, 0x3D, 0x02, 0x97, 0xDB, 0xEE, 0xA2, 0x81, 0x80, 0x00, 0x03, 0xAF, 0x01, 0x10, 0xF9, 0x80, 0x09, 0x01, 0x05, 0xF4, 0x21, 0x33, 0x04,
    0x37, 0x00, 0x10, 0x21, 0x84, 0x60, 0x6D, 0x20, 0x00, 0x05, 0xEF, 0x7C, 0x01, 0x10, 0x52, 0xDA, 0x20, 0x08, 0xE0, 0x05, 0x00, 0x02, 0xAF, 0x02, 0x10, 0x82, 0x54, 0x08, 0xEF, 0x04, 0xF8, 0xFF,
    0x9D, 0x03, 0x10, 0x32, 0x85, 0xC0, 0x30, 0x05, 0xFF, 0xDE, 0x5A, 0x01, 0x21, 0x95, 0xE2, 0x00, 0xB1, 0xE0, 0x01, 0x00, 0x20, 0x65, 0x00, 0xF7, 0x60, 0x0D, 0x06, 0xEF, 0x04, 0xF9, 0xDF, 0x05,
    0x21, 0x74, 0xE0, 0x01, 0x54, 0x04, 0xCE, 0x48, 0x01, 0x42, 0xC8, 0x60, 0x1A, 0xE0, 0x06, 0x00, 0x03, 0x9F, 0x01, 0x00, 0xF5, 0x80, 0x12, 0x04, 0x05, 0xF6, 0x8E, 0x21, 0xB6, 0x61, 0x62, 0x60,
    0x00, 0x04, 0xBE, 0x37, 0x10, 0x63, 0xEA, 0x60, 0x09, 0xE0, 0x07, 0x00, 0x03, 0x8F, 0x01, 0x00, 0xE4, 0x80, 0x13, 0x02, 0x18, 0xD2, 0x4D, 0xE2, 0x02, 0xE4, 0x20, 0x31, 0x00, 0x73, 0xE0, 0x00,
    0xA2, 0xE0, 0x05, 0x00, 0x00, 0x6F, 0xE2, 0x00, 0x96, 0x03, 0x2C, 0x70, 0x1B, 0xA2, 0xE0, 0x00, 0x1B, 0x03, 0xCF, 0x37, 0x00, 0x84, 0xA1, 0xA5, 0xE0, 0x08, 0x00, 0x02, 0x4E, 0x00, 0x00, 0xA0,
    0x28, 0x03, 0x8F, 0x10, 0x04, 0xC4, 0xC0, 0x1E, 0x03, 0xDF, 0x37, 0x01, 0x93, 0xE0, 0x0F, 0x31, 0x01, 0xFF, 0x3C, 0x22, 0xD8, 0x20, 0x06, 0x20, 0x00, 0x02, 0xEF, 0x05, 0x00, 0x82, 0xAE, 0x20,
    0x0A, 0x02, 0x39, 0x01, 0x20, 0xE0, 0x0D, 0xC7, 0x40, 0x00, 0x02, 0x2A, 0x00, 0x00, 0xC3, 0x87, 0x01, 0x5D, 0x41, 0xC3, 0x9E, 0x00, 0x4B, 0x22, 0x22, 0x01, 0x41, 0xD9, 0x40, 0x1E, 0xE0, 0x0B,
    0x00, 0x00, 0x06, 0x22, 0xAE, 0x22, 0x31, 0xE0, 0x03, 0x00, 0x08, 0xEF, 0x5C, 0x00, 0x10, 0x84, 0x37, 0x01, 0x10, 0x94, 0x62, 0x51, 0xE0, 0x08, 0x00, 0x00, 0xBF, 0x44, 0x2F, 0xC0, 0x8F, 0x80,
    0x00, 0x0B, 0xEF, 0x8D, 0x01, 0x21, 0xA5, 0xFE, 0xEF, 0x6A, 0x13, 0x00, 0x41, 0xC8, 0xE0, 0x0B, 0x34, 0x00, 0x6E, 0x22, 0xA3, 0x01, 0x00, 0xB3, 0x80, 0x2B, 0xA0, 0x00, 0x03, 0x9D, 0x03, 0x41,
    0xC7, 0x40, 0x0A, 0x05, 0xAD, 0x36, 0x02, 0x10, 0x74, 0xCA, 0xE0, 0x08, 0x34, 0x64, 0x61, 0x00, 0x00, 0xE5, 0x04, 0xB8, 0x03, 0xAD, 0x15, 0x30, 0xD7, 0x40, 0x31, 0x21, 0x36, 0x04, 0x7A, 0x24,
    0x01, 0x31, 0x85, 0x23, 0x7B, 0xE0, 0x04, 0x00, 0x84, 0xC7, 0x01, 0x10, 0xE8, 0xE0, 0x01, 0x14, 0x03, 0xAE, 0x26, 0x20, 0xB7, 0xE0, 0x00, 0x0D, 0x02, 0xBD, 0x68, 0x13, 0x43, 0x67, 0xE1, 0x03,
    0x6E, 0xA5, 0x2D, 0x00, 0x00, 0x23, 0xD8, 0xA0, 0x00, 0x03, 0xAE, 0x26, 0x10, 0xA6, 0xE2, 0x01, 0x34, 0x09, 0xFF, 0xEF, 0xAC, 0x58, 0x13, 0x00, 0x21, 0x54, 0x86, 0xB9, 0xC0, 0x35, 0x01, 0xDF,
    0x5A, 0x21, 0x4C, 0x40, 0x00, 0x00, 0x20, 0x82, 0xCC, 0x20, 0x31, 0x02, 0x25, 0x10, 0xA5, 0xE2, 0x05, 0xBB, 0x02, 0xDE, 0xBC, 0x69, 0x23, 0xE6, 0x02, 0x22, 0x43, 0x65, 0x24, 0x70, 0x00, 0xAA,
    0x24, 0x2E, 0x85, 0xC5, 0x20, 0x00, 0x00, 0x51, 0x85, 0xDA, 0x03, 0x8D, 0x24, 0x00, 0xA4, 0xE0, 0x05, 0x31, 0x20, 0x00, 0x03, 0xEE, 0xCD, 0x8B, 0x46, 0x20, 0x28, 0x04, 0x11, 0x11, 0x21, 0x22,
    0x11, 0x23, 0x7E, 0xE0, 0x00, 0x00, 0x08, 0x41, 0xC9, 0xFE, 0xEF, 0xAD, 0x47, 0x11, 0x00, 0xA4, 0xE0, 0x02, 0x95, 0xE0, 0x01, 0x00, 0x01, 0xEE, 0xDE, 0x24, 0x32, 0x02, 0x12, 0x11, 0x10, 0xE0,
    0x00, 0x2E, 0xE0, 0x01, 0x00, 0x01, 0x21, 0xC6, 0xE0, 0x01, 0x26, 0xE0, 0x0A, 0x00, 0x00, 0xAE, 0x20, 0x6C, 0xE0, 0x04, 0x00, 0x02, 0x21, 0x32, 0x74, 0xE2, 0x11, 0x57, 0x40, 0x00, 0x01, 0xEF,
    0x18, 0xE0, 0x04, 0x2F, 0x21, 0x69, 0x00, 0xA6, 0x24, 0xB3, 0xE0, 0x14, 0x00, 0x45, 0xF2, 0xE0, 0x05, 0x00, 0xE2, 0x00, 0xF9, 0xE0, 0x0E, 0x00, 0x01, 0xDF, 0x07, 0xE0, 0x05, 0x2F, 0x20, 0x00,
    0xC2, 0xD0, 0xE0, 0x0F, 0x00, 0x00, 0x7D, 0xA1, 0x91, 0xE0, 0x03, 0x00, 0x00, 0xF6, 0xE0, 0x0F, 0x2C, 0x60, 0x00, 0x00, 0xEF, 0x26, 0x23, 0xE0, 0x08, 0x00, 0xE7, 0x07, 0x8A, 0xE0, 0x05, 0x00,
    0x00, 0x8E, 0xE0, 0x0A, 0x64, 0x01, 0x00, 0x20, 0xC3, 0x61, 0xE0, 0x0C, 0x00, 0x47, 0x22, 0xE0, 0x09, 0x00, 0x00, 0xD3, 0xE0, 0x0C, 0x2B, 0x80, 0x00, 0x63, 0x58, 0xE0, 0x09, 0x00, 0xE1, 0x13,
    0x00, 0x00, 0x4C, 0xE0, 0x09, 0x2E, 0x40, 0x00, 0x01, 0x10, 0xE7, 0x80, 0x51, 0xE0, 0x0A, 0x00, 0x00, 0xEF, 0xE1, 0x09, 0x61, 0x80, 0x00, 0xA4, 0x9E, 0xE0, 0x0A, 0x00, 0x25, 0x54, 0xE0, 0x0D,
    0x00, 0x00, 0x40, 0x82, 0xD0, 0xE0, 0x0A, 0x00, 0x00, 0x2C, 0xE0, 0x0D, 0x30, 0x20, 0x00, 0xE8, 0x01, 0x67, 0xE0, 0x05, 0x00, 0x24, 0xE3, 0xE0, 0x0F, 0x00, 0x00, 0x81, 0xE2, 0x0C, 0xFB, 0xE0,
    0x12, 0x97, 0xE1, 0x11, 0x9A, 0x00, 0x3D, 0x20, 0x1B, 0xE0, 0x0F, 0x00, 0xA6, 0x05, 0xE0, 0x07, 0x00, 0xE2, 0x0B, 0x2C, 0xC0, 0x00, 0xE9, 0x07, 0xF2, 0x80, 0x00, 0x00, 0xBF, 0xE3, 0x07, 0x5B,
    0xE0, 0x03, 0x00, 0x00, 0x10, 0xE1, 0x0C, 0x33, 0x00, 0x5E, 0xE0, 0x03, 0x22, 0xE0, 0x08, 0x00, 0xE0, 0x0C, 0x99, 0xE2, 0x0D, 0x5F, 0xC0, 0x00, 0xE9, 0x02, 0x69, 0xE0, 0x00, 0x00, 0x00, 0xDF,
    0xE1, 0x11, 0x62, 0x40, 0x00, 0xE2, 0x0B, 0xCE, 0x00, 0x9F, 0xE0, 0x13, 0xCA, 0x20, 0x00, 0xE6, 0x05, 0x06, 0x60, 0x00, 0xEB, 0x0F, 0xBC, 0xC0, 0x00, 0xE5, 0x01, 0x6C, 0xE0, 0x00, 0x00, 0x26,
    0xB6, 0xE0, 0x14, 0x00, 0x00, 0x40, 0xE5, 0x08, 0x2E, 0x00, 0xDF, 0x26, 0xB6, 0xE0, 0x14, 0x00, 0xA8, 0x6C, 0xE0, 0x02, 0x00, 0xE1, 0x14, 0x95, 0x60, 0x00, 0xE4, 0x08, 0x9C, 0xE4, 0x09, 0xBF,
    0xE0, 0x07, 0x00, 0x4C, 0x46, 0xE0, 0x04, 0x00, 0xE2, 0x11, 0xF8, 0xC0, 0x00, 0xEB, 0x02, 0x36, 0x60, 0x00, 0x01, 0xEF, 0x08, 0xC0, 0x19, 0xE0, 0x11, 0x00, 0xE3, 0x06, 0xCE, 0x00, 0xDF, 0x47,
    0x81, 0xE0, 0x16, 0x00, 0x00, 0xC2, 0x60, 0x5C, 0xE0, 0x00, 0x00, 0x00, 0xAF, 0xE0, 0x16, 0x2E, 0x40, 0x00, 0x00, 0x80, 0xE3, 0x05, 0x67, 0x00, 0x5F, 0x40, 0x13, 0xE0, 0x16, 0x00, 0x00, 0x30,
    0xE1, 0x05, 0x99, 0x00, 0x1D, 0xE0, 0x16, 0x2E, 0x60, 0x00, 0xAA, 0x39, 0xA0, 0x00, 0x00, 0x09, 0x60, 0x13, 0xE0, 0x16, 0x00, 0x2A, 0x64, 0xE0, 0x02, 0x00, 0x2C, 0x15, 0xE0, 0x19, 0x00, 0x00,
    0xD0, 0xE0, 0x02, 0x30, 0x01, 0xFF, 0xCF, 0xE4, 0x0F, 0xC1, 0xE0, 0x04, 0x00, 0x00, 0x70, 0x20, 0x29, 0xE0, 0x00, 0x00, 0x00, 0x7F, 0xE0, 0x04, 0x1A, 0xE0, 0x0F, 0x00, 0x00, 0x30, 0xE1, 0x03,
    0x32, 0x00, 0x3E, 0xE0, 0x0F, 0x25, 0xE0, 0x04, 0x00, 0xE4, 0x04, 0x00, 0x00, 0x2B, 0xE0, 0x04, 0x1A, 0xE0, 0x0F, 0x00, 0xE2, 0x04, 0xCC, 0xE7, 0x07, 0x86, 0xE0, 0x0E, 0x00, 0xAB, 0x06, 0x40,
    0x00, 0xE5, 0x12, 0x5A, 0xE0, 0x04, 0x00, 0x00, 0xE3, 0x40, 0x2C, 0x80, 0x00, 0x00, 0xCF, 0xE2, 0x1A, 0x61, 0x40, 0x00, 0xE2, 0x02, 0x65, 0xE6, 0x10, 0x25, 0xE0, 0x06, 0x00, 0x00, 0xA1, 0x80,
    0x61, 0x40, 0x00, 0x01, 0x6F, 0x00, 0x01, 0x00, 0x00, 0xE0, 0x1B, 0x00, 0x01, 0x81, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x4D, 0xE0, 0x1B, 0x2F, 0x20, 0x00, 0x00, 0x71, 0xE0, 0x00, 0x31, 0x01, 0xFF,
    0x2A, 0x20, 0x0E, 0xE0, 0x1B, 0x00, 0xE0, 0x02, 0x32, 0x00, 0x18, 0xE0, 0x1B, 0x2F, 0x20, 0x00, 0xE0, 0x01, 0x32, 0x01, 0xEF, 0x06, 0x20, 0x0E, 0xE0, 0x1B, 0x00, 0xE0, 0x01, 0xCB, 0x01, 0xCF,
    0x04, 0xE0, 0x1B, 0x2F, 0x20, 0x00, 0xE0, 0x01, 0x32, 0x01, 0xAF, 0x01, 0x20, 0x0E, 0xE0, 0x1B, 0x00, 0x00, 0x91, 0x20, 0xF7, 0x80, 0x00, 0x00, 0x7E, 0xE0, 0x1B, 0x2E, 0x40, 0x00, 0x00, 0xA2,
    0x80, 0x2F, 0x20, 0x00, 0x00, 0x4C, 0x40, 0x0E, 0xE0, 0x1B, 0x00, 0x00, 0xB2, 0x20, 0x2C, 0x80, 0x00, 0x00, 0x1B, 0xE0, 0x1B, 0x2E, 0x40, 0x00, 0x00, 0xC2, 0x80, 0x2F, 0x21, 0x30, 0x00, 0x08,
    0x40, 0x0E, 0xE0, 0x1B, 0x00, 0x00, 0xD2, 0x20, 0x2D, 0x60, 0x00, 0x01, 0xDF, 0x03, 0xE0, 0x1B, 0x2E, 0x40, 0x00, 0x00, 0xE3, 0x60, 0x2F, 0x20, 0x00, 0x00, 0xAF, 0x40, 0x0D, 0xE0, 0x1C, 0x00,
    0x00, 0xF4, 0x20, 0x2D, 0x60, 0x00, 0x00, 0x6E, 0xE0, 0x1C, 0x2E, 0x40, 0x00, 0x00, 0xF5, 0x60, 0x2F, 0x20, 0x00, 0x00, 0x1C, 0x40, 0x0D, 0xE0, 0x1C, 0x00, 0x00, 0xF7, 0x20, 0x2D, 0x60, 0x00,
    0x00, 0x07, 0xE0, 0x1C, 0x2E, 0x20, 0x00, 0x01, 0x10, 0xF9, 0x60, 0x2F, 0x21, 0x30, 0x00, 0x02, 0x20, 0x0D, 0xE0, 0x1C, 0x00, 0x01, 0x10, 0xFB, 0x20, 0x2E, 0x40, 0x00, 0x00, 0x9F, 0xE0, 0x1C,
    0x2E, 0x40, 0x00, 0x01, 0x10, 0xFD, 0x40, 0x2F, 0x20, 0x00, 0x00, 0x3E, 0x40, 0x0D, 0xE0, 0x1C, 0x00, 0x01, 0x20, 0xFE, 0x20, 0x2E, 0x40, 0x00, 0x00, 0x19, 0xE0, 0x1C, 0x2E, 0x40, 0x00, 0x00,
    0x40, 0x40, 0x2E, 0x20, 0x00, 0x00, 0xEF, 0xE1, 0x20, 0xC8, 0x01, 0x00, 0x60, 0x20, 0x2E, 0x40, 0x00, 0x00, 0x8F, 0xE2, 0x1F, 0xF9, 0x02, 0x00, 0x00, 0x90, 0x40, 0x2F, 0x20, 0x00, 0x00, 0x3D,
    0x20, 0x0B, 0xE0, 0x1E, 0x00, 0x00, 0xC0, 0x20, 0x2E, 0x20, 0x00, 0x01, 0xEF, 0x17, 0xE0, 0x1E, 0x2F, 0x20, 0x00, 0x00, 0xE1, 0x20, 0x2F, 0x20, 0x00, 0x00, 0xBF, 0xE0, 0x21, 0xCA, 0x01, 0x00,
    0xF3, 0x20, 0x2F, 0x20, 0x00, 0x00, 0x6D, 0x20, 0x0A, 0xE0, 0x1F, 0x00, 0x00, 0xF6, 0x20, 0x2F, 0x20, 0x00, 0x00, 0x29, 0xE0, 0x1F, 0x2F, 0x20, 0x00, 0x00, 0xFA, 0x20, 0x2F, 0xE4, 0x22, 0x5C,
    0x40, 0x00, 0x81, 0xFC, 0xE4, 0x20, 0x29, 0x20, 0x00, 0x00, 0x40, 0x81, 0xFC, 0x00, 0x09, 0x20, 0x0A, 0xE0, 0x1F, 0x00, 0x00, 0x70, 0x60, 0x32, 0x00, 0xBF, 0xE2, 0x20, 0xC5, 0x20, 0x00, 0x00,
    0xB0, 0x20, 0xC8, 0xE5, 0x21, 0xF2, 0x60, 0x00, 0x00, 0xD3, 0x20, 0x30, 0xE5, 0x21, 0x8C, 0x60, 0x00, 0x00, 0xE6, 0x20, 0x31, 0x01, 0xFF, 0x8E, 0xE2, 0x21, 0x5F, 0x23, 0x2D, 0x61, 0x63, 0xE1,
    0x23, 0x94, 0x02, 0x00, 0x30, 0xFC, 0x20, 0x64, 0x00, 0xAF, 0xE0, 0x23, 0xFC, 0x81, 0x30, 0x00, 0x3A, 0x20, 0x08, 0xE0, 0x21, 0x00, 0x00, 0xA1, 0x20, 0x64, 0xE2, 0x23, 0x91, 0x20, 0x00, 0x00,
    0xD5, 0x20, 0x32, 0xE0, 0x25, 0x64, 0x04, 0x10, 0xF8, 0xFF, 0xFF, 0xAE, 0xE0, 0x24, 0xC9, 0x04, 0x00, 0x40, 0xFC, 0xFF, 0xEF, 0xE1, 0x24, 0x2E, 0x66, 0xF4, 0xE1, 0x24, 0x93, 0x20, 0x00, 0x03,
    0xC4, 0xFF, 0xCF, 0x16, 0x20, 0x06, 0xE0, 0x22, 0x00, 0x25, 0x5C, 0x00, 0x5B, 0xE0, 0x22, 0x2E, 0x40, 0x00, 0x02, 0x50, 0xFE, 0x8E, 0xE3, 0x22, 0xF2, 0x60, 0x00, 0x02, 0xC2, 0xCF, 0x15, 0x60,
    0x07, 0xE0, 0x22, 0x00, 0x01, 0xE9, 0x28, 0xE0, 0x22, 0x2C, 0x60, 0x00, 0x00, 0x60, 0xE1, 0x26, 0x5F, 0x20, 0x00, 0x00, 0x31, 0x20, 0x03, 0xE0, 0xFF, 0x00, 0xE0, 0x87, 0x00, 0x01, 0x10, 0x11,
    0x20, 0x91, 0x60, 0x04, 0xE0, 0x13, 0x00, 0x04, 0x42, 0x44, 0x34, 0x23, 0x12, 0xA0, 0x20, 0x08, 0x20, 0x95, 0x8A, 0x14, 0x00, 0x10, 0x73, 0x9A, 0x26, 0xA0, 0x0F, 0xE0, 0x0E, 0x00, 0x28, 0x81,
    0x02, 0xCE, 0x7A, 0x24, 0x80, 0x1C, 0x08, 0xA3, 0xEE, 0xED, 0x8E, 0x01, 0x51, 0xED, 0xDD, 0xCE, 0xEA, 0x15, 0x89, 0x05, 0xF7, 0x9C, 0xA9, 0xBA, 0xFD, 0x9D, 0x62, 0xC3, 0x0A, 0x20, 0xE9, 0x6B,
    0x75, 0xEC, 0x17, 0xC4, 0x9D, 0x65, 0xE9, 0x3B, 0x80, 0x42, 0xE0, 0x0E, 0x00, 0x06, 0xE7, 0x07, 0x10, 0x32, 0x75, 0xEB, 0x8D, 0x44, 0x25, 0x0A, 0x50, 0xBD, 0x03, 0x00, 0xE5, 0x4B, 0xE9, 0x18,
    0x00, 0x92, 0x7E, 0xE5, 0x14, 0x63, 0x00, 0xE7, 0x49, 0x12, 0x02, 0x73, 0xEC, 0x39, 0x20, 0x51, 0x0A, 0x70, 0x7F, 0x01, 0x00, 0xB2, 0x7D, 0xDB, 0x03, 0x00, 0x50, 0x9E, 0x40, 0x41, 0xE0, 0x10,
    0x00, 0x60, 0x32, 0x01, 0x10, 0xB4, 0x40, 0x24, 0x01, 0x70, 0x7F, 0x2A, 0xC1, 0x04, 0x7D, 0xCB, 0x03, 0x00, 0x40, 0x40, 0x0D, 0xE0, 0x11, 0x00, 0x60, 0x32, 0x02, 0x00, 0x30, 0xEB, 0x2B, 0xE1,
    0x00, 0x50, 0x24, 0xC4, 0x05, 0xD3, 0x5C, 0xEA, 0x05, 0x00, 0x71, 0xE8, 0x15, 0x5C, 0x80, 0x32, 0x02, 0x00, 0xC4, 0x5E, 0x40, 0x37, 0x08, 0x29, 0x40, 0xEC, 0x18, 0xE7, 0x5D, 0x10, 0xE6, 0x5D,
    0x20, 0x11, 0xE0, 0x11, 0x00, 0xA0, 0x32, 0x01, 0x60, 0xCE, 0x20, 0xBE, 0x00, 0xC4, 0x45, 0x2D, 0x03, 0x81, 0xFE, 0xEE, 0xDF, 0xEC, 0x15, 0x21, 0x40, 0x32, 0x21, 0x96, 0x01, 0x10, 0xFA, 0x21,
    0x06, 0x08, 0x30, 0xC8, 0xBC, 0x17, 0x00, 0x10, 0xB6, 0xCC, 0x5A, 0xE0, 0x11, 0x61, 0x40, 0x00, 0x40, 0x32, 0x00, 0xAB, 0x24, 0x8C, 0x01, 0xD5, 0x2D, 0x20, 0x0D, 0x00, 0x10, 0x61, 0x5C, 0xC2,
    0x0C, 0xE0, 0x0F, 0x00, 0x40, 0x32, 0x01, 0xFB, 0x3C, 0x26, 0x28, 0x00, 0x6F, 0xE0, 0x0F, 0x21, 0xE0, 0x08, 0x00, 0x60, 0x32, 0x2C, 0x78, 0x01, 0x50, 0xCE, 0xE0, 0x05, 0x1A, 0x60, 0x6F, 0xE0,
    0x0D, 0x00, 0x60, 0x32, 0x0C, 0xED, 0x06, 0x00, 0x10, 0xEC, 0x03, 0x10, 0x53, 0x55, 0x55, 0x55, 0x13, 0x52, 0x20, 0x04, 0x08, 0x24, 0x00, 0x30, 0x69, 0x02, 0x10, 0x84, 0x99, 0x37, 0x20, 0x33,
    0x06, 0x20, 0x78, 0x03, 0x00, 0x73, 0x99, 0x47, 0xE0, 0x02, 0x42, 0x60, 0x32, 0x0C, 0xF8, 0x1A, 0x00, 0x00, 0xE8, 0x08, 0x20, 0xD9, 0xDD, 0xDD, 0xDD, 0x19, 0xC6, 0x20, 0x04, 0x09, 0x4B, 0x00,
    0xB2, 0xEF, 0x3A, 0x82, 0xED, 0xFF, 0xBE, 0x05, 0x2C, 0xD1, 0x05, 0xEE, 0x5B, 0x71, 0xEC, 0xFF, 0xCE, 0xE9, 0x02, 0xA1, 0x60, 0x32, 0x00, 0xD4, 0x29, 0xE5, 0x14, 0xE4, 0x0C, 0x20, 0xEA, 0xAB,
    0xAA, 0xFB, 0x2B, 0xE7, 0xAC, 0xAA, 0xDA, 0x5D, 0x10, 0xFA, 0xBC, 0xBE, 0xEA, 0x7D, 0x54, 0xE9, 0x21, 0xA0, 0x06, 0xE8, 0xBD, 0xCE, 0xDA, 0x8E, 0x54, 0xD8, 0xE2, 0x02, 0x76, 0x60, 0x32, 0x00,
    0x93, 0x2B, 0xE1, 0x1F, 0xD1, 0x2E, 0x20, 0xDA, 0x14, 0x11, 0xE3, 0x2B, 0xE7, 0x17, 0x11, 0x92, 0x5D, 0x91, 0xDE, 0x32, 0xFA, 0xCF, 0x14, 0x00, 0x71, 0xBE, 0x03, 0x60, 0xDE, 0x25, 0xE8, 0xEF,
    0x15, 0x00, 0x51, 0xDD, 0xE3, 0x01, 0x10, 0x60, 0x32, 0x00, 0x63, 0x2B, 0x49, 0x05, 0xA0, 0x6E, 0x20, 0xDA, 0x04, 0x00, 0x40, 0x32, 0x06, 0x00, 0x91, 0x6D, 0xE8, 0x2D, 0x00, 0xC4, 0x2D, 0x26,
    0x06, 0x10, 0xF9, 0x17, 0xE5, 0x5E, 0x00, 0xA2, 0x22, 0x19, 0x01, 0x00, 0xE7, 0xEB, 0x01, 0x06, 0x60, 0x32, 0x00, 0x33, 0x21, 0x32, 0x01, 0x60, 0x8F, 0xE0, 0x01, 0x32, 0x04, 0xBE, 0xEE, 0x04,
    0x00, 0x61, 0x20, 0xC7, 0x06, 0x00, 0xE5, 0x6A, 0xFD, 0x07, 0x00, 0x50, 0x46, 0xA8, 0x00, 0xC3, 0x41, 0xB7, 0x80, 0x00, 0x60, 0x32, 0x05, 0x23, 0xBE, 0x02, 0x00, 0x30, 0xBF, 0xE0, 0x01, 0x32,
    0x00, 0xCE, 0x42, 0x3E, 0x01, 0x32, 0x13, 0x20, 0x28, 0x01, 0x9C, 0xFD, 0x20, 0x90, 0x00, 0x31, 0x20, 0x09, 0x01, 0xA1, 0x3F, 0x80, 0x2F, 0x20, 0x00, 0x80, 0x32, 0x05, 0xCD, 0x03, 0x00, 0x10,
    0xCF, 0x21, 0xE0, 0x00, 0x98, 0x0C, 0x7D, 0xFA, 0x05, 0x00, 0x20, 0xC7, 0x7C, 0x01, 0x00, 0xA2, 0x4D, 0xE8, 0x09, 0x22, 0x42, 0x04, 0x9D, 0x02, 0x00, 0x90, 0x5F, 0x20, 0x2C, 0x80, 0x00, 0x60,
    0x32, 0x01, 0x13, 0xCB, 0x20, 0x4B, 0x01, 0xDD, 0x23, 0xE0, 0x00, 0x32, 0x02, 0x5D, 0xE7, 0x08, 0x22, 0xA8, 0x01, 0xED, 0x03, 0x20, 0x32, 0x06, 0xE3, 0x0C, 0x00, 0x40, 0xDC, 0xEC, 0x07, 0xE0,
    0x09, 0x32, 0x00, 0xDA, 0x20, 0xB8, 0x01, 0xDC, 0x24, 0xE0, 0x01, 0x32, 0x06, 0xD6, 0x19, 0x00, 0x71, 0x7F, 0xEA, 0x06, 0x20, 0x32, 0x08, 0xE2, 0x0D, 0x00, 0x60, 0xAD, 0xE7, 0x0A, 0x00, 0x80,
    0xE2, 0x01, 0x50, 0x80, 0x65, 0x00, 0xD9, 0x20, 0x32, 0x01, 0xDA, 0x25, 0xE0, 0x01, 0x32, 0x00, 0xD5, 0x20, 0x32, 0x01, 0x6F, 0xE8, 0xC0, 0x32, 0x01, 0x8D, 0xE5, 0x40, 0x32, 0x01, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x60, 0x00, 0x1A, 0xE7, 0x07, 0x00, 0x00, 0xFB, 0x13, 0xE8, 0x06, 0x00, 0x00, 0xE9, 0x26, 0xDA, 0x04, 0x00, 0xE3, 0x2B, 0xE7, 0x17, 0x00, 0x91, 0x5D, 0xD5, 0x19, 0x00, 0x71,
    0x6F, 0x20, 0x14, 0x0B, 0xA2, 0x4D, 0xE2, 0x0D, 0x00, 0x60, 0x8D, 0xE5, 0x0A, 0x00, 0x80, 0x6F, 0x60, 0x2E, 0x40, 0x00, 0xE0, 0x01, 0x32, 0x01, 0xE8, 0x27, 0xE0, 0x24, 0x32, 0x40, 0x38, 0x01,
    0xE7, 0x28, 0xE0, 0x28, 0x32, 0x00, 0xE6, 0xE0, 0xFF, 0x32, 0xE1, 0x1C, 0x97, 0x61, 0x64, 0xE1, 0x29, 0x97, 0xE1, 0x26, 0xCA, 0x22, 0x1B, 0xE2, 0x27, 0x30, 0x00, 0xD8, 0x20, 0x32, 0x01, 0xDA,
    0x25, 0xE1, 0x24, 0xFD, 0x05, 0xD9, 0x05, 0x00, 0x00, 0xDB, 0x24, 0xE0, 0x24, 0x32, 0x00, 0xDA, 0x20, 0x32, 0x01, 0xDD, 0x23, 0xE0, 0x24, 0x32, 0x05, 0xCB, 0x04, 0x00, 0x10, 0xCF, 0x21, 0xE0,
    0x23, 0x32, 0x06, 0x23, 0xCC, 0x03, 0x00, 0x30, 0xBF, 0x20, 0xE0, 0x24, 0x32, 0x04, 0xBE, 0x02, 0x00, 0x50, 0x9F, 0xE0, 0x24, 0x32, 0x05, 0x43, 0xAF, 0x00, 0x00, 0x80, 0x7E, 0xE0, 0x24, 0x32,
    0x05, 0x73, 0x8E, 0x00, 0x00, 0xB0, 0x4E, 0xE0, 0x24, 0x32, 0x05, 0xB3, 0x5D, 0x00, 0x00, 0xE2, 0x1D, 0xE0, 0x24, 0x32, 0x05, 0xF5, 0x2C, 0x00, 0x00, 0xE5, 0x0A, 0xE0, 0x24, 0x32, 0x05, 0xEB,
    0x08, 0x00, 0x00, 0xF9, 0x06, 0xE0, 0x24, 0x32, 0x05, 0xDF, 0x03, 0x00, 0x20, 0xEC, 0x02, 0xE0, 0x24, 0x32, 0x05, 0x7E, 0x00, 0x00, 0x60, 0xBE, 0x00, 0xE0, 0x23, 0x32, 0x05, 0xEB, 0x17, 0x00,
    0x00, 0xA1, 0x5F, 0xE0, 0x24, 0x32, 0x05, 0x58, 0x01, 0x00, 0x00, 0xD6, 0x1C, 0xE0, 0x24, 0x32, 0x20, 0x31, 0x02, 0x20, 0xFA, 0x06, 0xE0, 0x24, 0x32, 0x20, 0x00, 0x02, 0x71, 0xAE, 0x02, 0xE0,
    0x27, 0x32, 0x01, 0xD6, 0x4D, 0x20, 0x69, 0xE2, 0x22, 0x96, 0x04, 0x00, 0x00, 0x51, 0xED, 0x16, 0xE0, 0x26, 0x32, 0x02, 0x20, 0xD6, 0x7E, 0x40, 0xCF, 0xE0, 0x22, 0x65, 0x02, 0x31, 0xA5, 0xDE,
    0x21, 0x66, 0x20, 0x32, 0x00, 0x03, 0xE6, 0x1C, 0x5F, 0x06, 0xF7, 0x6B, 0x66, 0x87, 0xB9, 0xEE, 0x5A, 0x20, 0x64, 0x01, 0x00, 0x30, 0x23, 0x68, 0x26, 0x92, 0x16, 0x7A, 0x66, 0xC7, 0x5D, 0xD5,
    0x7C, 0x66, 0xB7, 0x6F, 0xE8, 0x6A, 0x66, 0xC7, 0x4D, 0xE2, 0x6E, 0x66, 0xA6, 0x8E, 0xE5, 0x6C, 0x66, 0xB6, 0xE6, 0x01, 0x92, 0x06, 0xF7, 0xEF, 0xEE, 0xFF, 0xCE, 0x69, 0x02, 0x20, 0x31, 0x0C,
    0x00, 0x82, 0x9E, 0x01, 0x00, 0xE3, 0x2A, 0xE7, 0xEF, 0xEE, 0xFE, 0x5D, 0xD5, 0x20, 0x04, 0x0B, 0x6F, 0xE8, 0xEE, 0xEE, 0xFE, 0x4D, 0xE2, 0xEF, 0xEE, 0xEE, 0x8F, 0xF5, 0x40, 0x0E, 0x20, 0x21,
    0x80, 0x00, 0x04, 0x73, 0x77, 0x77, 0x67, 0x34, 0x60, 0x31, 0x01, 0x40, 0xEC, 0x23, 0x02, 0x01, 0xF4, 0x1A, 0x20, 0x10, 0x07, 0x77, 0x26, 0x62, 0x66, 0x66, 0x66, 0x26, 0x63, 0x40, 0x04, 0x00,
    0x61, 0x20, 0x04, 0x00, 0x46, 0x60, 0x0E, 0x80, 0x2F, 0x20, 0x00, 0x02, 0x10, 0x11, 0x11, 0x40, 0x94, 0x20, 0x00, 0x02, 0x60, 0xCF, 0x15, 0x26, 0x97, 0x00, 0x08, 0x20, 0x10, 0xC0, 0x11, 0xE0,
    0x18, 0x00, 0x00, 0x60, 0x23, 0x9A, 0x01, 0x10, 0xDB, 0x24, 0x9D, 0xE0, 0x21, 0x00, 0x40, 0x32, 0x01, 0x70, 0xBF, 0xA0, 0x72, 0xE0, 0x1D, 0x00, 0x20, 0x32, 0x01, 0x10, 0xE5, 0x20, 0xCE, 0xE0,
    0x22, 0x00, 0x04, 0x60, 0x8E, 0x10, 0x73, 0xEE, 0x28, 0x05, 0xE0, 0x22, 0x00, 0x04, 0x60, 0xAF, 0x75, 0xEB, 0x7D, 0xE0, 0x24, 0x97, 0x04, 0x00, 0x60, 0xEF, 0xFE, 0xAD, 0xE0, 0x24, 0xFC, 0x05,
    0x00, 0x00, 0x50, 0xAB, 0x79, 0x24, 0x20, 0x06, 0xE0, 0x23, 0x00, 0x01, 0x10, 0x22, 0xE0, 0x25, 0x95, 0xE0, 0x50, 0x00, 0x01, 0x00, 0x00,
};

static const painter_compressed_image_descriptor_t gfx_djinn_compressed PROGMEM = {
    .base = {
        .image_format = IMAGE_FORMAT_GRAYSCALE,
        .compression  = IMAGE_COMPRESSED_LZF,
        .width        = 102,
        .height       = 288
    },
    .image_bpp       = 4,
    .image_palette   = NULL,
    .chunk_count     = 4,
    .chunk_size      = 4096,
    .chunk_offsets   = gfx_djinn_chunk_offsets,
    .compressed_data = gfx_djinn_chunk_data,
    .compressed_size = 4759 // original = 14688 bytes (4bpp) / 32.40% of original // rgb24 = 88128 bytes / 5.40% of rgb24
};

painter_image_t gfx_djinn PROGMEM = (painter_image_t)&gfx_djinn_compressed;

// clang-format on
