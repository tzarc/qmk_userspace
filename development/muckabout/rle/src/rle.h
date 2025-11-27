// Copyright 2018-2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "stream.h"

bool rle_decode(stream_t *in_stream, stream_t *out_stream);

#ifdef RLE_ENCODER
bool rle_encode(stream_t *in_stream, stream_t *out_stream);
#endif // RLE_ENCODER
