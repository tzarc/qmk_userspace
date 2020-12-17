/*
Copyright 2015 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x6465
#define DEVICE_VER      0x0001
#define MANUFACTURER    none
#define PRODUCT         zzz
#define DESCRIPTION     test board for qmk

#define MATRIX_ROWS 1
#define MATRIX_COLS 1

#define MATRIX_COL_PINS { B0 }
#define MATRIX_ROW_PINS { A7 }

#define DIODE_DIRECTION COL2ROW

#define DEBOUNCE    5
