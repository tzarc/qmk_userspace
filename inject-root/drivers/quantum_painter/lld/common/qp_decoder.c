/* Copyright 2018-2020 Nick Brassel (@tzarc)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include <lzf.h>
#include <qp_decoder.h>

uint32_t qp_decode(const void* const input_buffer, const uint32_t input_size, void* output_buffer, const uint32_t output_size) {
    // Use LZF decompressor to decode the chunk data
    return (uint32_t)lzf_decompress(input_buffer, input_size, output_buffer, output_size);
}