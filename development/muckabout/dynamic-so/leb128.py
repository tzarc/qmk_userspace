#!/usr/bin/env python3
# Copyright 2018-2024 Nick Brassel (@tzarc)
# SPDX-License-Identifier: GPL-2.0-or-later

# Equivalent C++ code can be found here: https://llvm.org/doxygen/LEB128_8h_source.html


def leb128_encode_unsigned(i: int) -> bytearray:
    r = []
    while True:
        byte = i & 0x7F
        i = i >> 7
        if i == 0:
            r.append(byte)
            return bytearray(r)
        r.append(0x80 | byte)


def leb128_decode_unsigned(b: bytearray) -> int:
    r = 0
    for i, e in enumerate(b):
        r = r + ((e & 0x7F) << (i * 7))
    return r


def leb128_encode_signed(i: int) -> bytearray:
    r = []
    while True:
        byte = i & 0x7F
        i = i >> 7
        if (i == 0 and byte & 0x40 == 0) or (i == -1 and byte & 0x40 != 0):
            r.append(byte)
            return bytearray(r)
        r.append(0x80 | byte)


def leb128_decode_signed(b: bytearray) -> int:
    r = 0
    for i, e in enumerate(b):
        r = r + ((e & 0x7F) << (i * 7))
    if e & 0x40 != 0:
        r |= -(1 << (i * 7) + 7)
    return r
