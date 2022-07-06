// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <qp.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Quantum Painter RGB565 surface device
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Factory method for creating a new surface
painter_device_t qp_rgb565_surface_make_device(uint16_t width, uint16_t height);

// Get access to the buffer and its length
const void* qp_rgb565_surface_get_buffer_ptr(painter_device_t device);
uint32_t    qp_rgb565_surface_get_pixel_count(painter_device_t device);