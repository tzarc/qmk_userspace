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

#pragma once

#include_next <board.h>

#define BOARD_MATRIX_NOAH
#define STM32_VDD                   300U

#undef STM32_HSECLK
#define STM32_HSECLK                8000000U

#undef STM32_LSECLK
#define STM32_LSECLK                0U

#define LINE_ROW_1 B0
#define LINE_ROW_2 A1
#define LINE_ROW_3 C14
#define LINE_ROW_4 C13
#define LINE_ROW_5 A0

#define LINE_COL_1 C15
#define LINE_COL_2 B10
#define LINE_COL_3 B7
#define LINE_COL_4 B6
#define LINE_COL_5 B5
#define LINE_COL_6 B4
#define LINE_COL_7 A15
#define LINE_COL_8 A10
#define LINE_COL_9 A9
#define LINE_COL_10 A8
#define LINE_COL_11 B15
#define LINE_COL_12 B14
#define LINE_COL_13 B13
#define LINE_COL_14 B12
#define LINE_COL_15 B2
