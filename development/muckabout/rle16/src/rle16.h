// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "stream16.h"

bool rle16_decode(stream16_t* in_stream, stream16_t* out_stream, void* ctx);

#ifdef RLE_ENCODER
bool rle16_encode(stream16_t* in_stream, stream16_t* out_stream, void* ctx);
#endif // RLE_ENCODER
