// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <string.h>
#include <stdlib.h>
#include <utf8.h>
#include <qp.h>
#include <qp_rgb565_surface.h>
#include <qp_internal.h>
#include <qp_fallback.h>
#include <qp_utils.h>

#ifdef PROTOCOL_CHIBIOS
#    include <ch.h>
#    if !defined(CH_CFG_USE_MEMCORE) || CH_CFG_USE_MEMCORE == FALSE
#        error ChibiOS is configured without a memory allocator. Your keyboard may have set `#define CH_CFG_USE_MEMCORE FALSE`, which is incompatible with this debounce algorithm.
#    endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Device definition
typedef struct qmk_rgb565_surface_device_t {
    struct painter_driver_t qp_driver;  // must be first, so it can be cast from the painter_device_t* type

    // Geometry and buffer
    uint16_t  width;
    uint16_t  height;
    uint16_t *buffer;

    // Manually manage the viewport for streaming pixel data to the display
    uint16_t viewport_l;
    uint16_t viewport_t;
    uint16_t viewport_r;
    uint16_t viewport_b;

    // Current write location to the display when streaming pixel data
    uint16_t pixdata_x;
    uint16_t pixdata_y;
} qmk_rgb565_surface_device_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers
//
// NOTE: The variables in this section are intentionally outside a stack frame. They are able to be defined with larger
//       sizes than the normal stack frames would allow, and as such need to be external.
//
//       **** DO NOT refactor this and decide to place the variables inside the function calling them -- you will ****
//       **** very likely get artifacts rendered to the screen as a result.                                       ****
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Static buffer to contain a generated color palette
#if QUANTUM_PAINTER_SUPPORTS_256_PALETTE
static qp_pixel_color_t hsv_lookup_table[256];
static uint16_t         rgb565_palette[256];
#else
static qp_pixel_color_t hsv_lookup_table[16];
static uint16_t         rgb565_palette[16];
#endif

#define BYTE_SWAP(x) (((((uint16_t)(x)) >> 8) & 0x00FF) | ((((uint16_t)(x)) << 8) & 0xFF00))

// Color conversion to RGB565
static inline uint16_t rgb_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t rgb565 = (((uint16_t)r) >> 3) << 11 | (((uint16_t)g) >> 2) << 5 | (((uint16_t)b) >> 3);
    return BYTE_SWAP(rgb565);
}

// Color conversion to RGB565
static inline uint16_t hsv_to_rgb565(uint8_t hue, uint8_t sat, uint8_t val) {
    RGB rgb = hsv_to_rgb_nocie((HSV){hue, sat, val});
    return rgb_to_rgb565(rgb.r, rgb.g, rgb.b);
}

static inline void increment_pixdata_location(qmk_rgb565_surface_device_t *surf) {
    // Increment the X-position
    surf->pixdata_x++;

    // If the x-coord has gone past the right-side edge, loop it back around and increment the y-coord
    if (surf->pixdata_x > surf->viewport_r) {
        surf->pixdata_x = surf->viewport_l;
        surf->pixdata_y++;
    }

    // If the y-coord has gone past the bottom, loop it back to the top
    if (surf->pixdata_y > surf->viewport_b) {
        surf->pixdata_y = surf->viewport_t;
    }
}

static inline void setpixel(qmk_rgb565_surface_device_t *surf, uint16_t x, uint16_t y, uint16_t color) { surf->buffer[y * surf->width + x] = color; }

static inline void append_pixel(qmk_rgb565_surface_device_t *surf, uint16_t pixel) {
    setpixel(surf, surf->pixdata_x, surf->pixdata_y, pixel);
    increment_pixdata_location(surf);
}

static inline void stream_pixdata(qmk_rgb565_surface_device_t *surf, const uint16_t *data, uint32_t native_pixel_count) {
    for (uint32_t pixel_counter = 0; pixel_counter < native_pixel_count; ++pixel_counter) {
        append_pixel(surf, data[pixel_counter]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Palette / Monochrome-format image rendering
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Palette renderer
static inline void stream_palette_pixdata_impl(qmk_rgb565_surface_device_t *surf, const uint16_t *const rgb565_palette, uint8_t bits_per_pixel, uint32_t pixel_count, const void *const pixel_data, uint32_t byte_count) {
    const uint8_t  pixel_bitmask    = (1 << bits_per_pixel) - 1;
    const uint8_t  pixels_per_byte  = 8 / bits_per_pixel;
    const uint8_t *pixdata          = (const uint8_t *)pixel_data;
    uint32_t       remaining_pixels = pixel_count;  // don't try to derive from byte_count, we may not use an entire byte

    // Transmit each block of pixels
    while (remaining_pixels > 0) {
        for (uint16_t p = 0; p < pixel_count; p += pixels_per_byte) {
            uint8_t pixval      = *pixdata;
            uint8_t loop_pixels = remaining_pixels < pixels_per_byte ? remaining_pixels : pixels_per_byte;
            for (uint8_t q = 0; q < loop_pixels; ++q) {
                append_pixel(surf, rgb565_palette[pixval & pixel_bitmask]);
            }
            ++pixdata;
            remaining_pixels -= loop_pixels;
        }
    }
}

// Recolored renderer
static inline void stream_palette_pixdata(qmk_rgb565_surface_device_t *surf, const uint8_t *const rgb_palette, uint8_t bits_per_pixel, uint32_t pixel_count, const void *const pixel_data, uint32_t byte_count) {
    // Generate the color lookup table
    uint16_t items = 1 << bits_per_pixel;  // number of items we need to interpolate
    for (uint16_t i = 0; i < items; ++i) {
        rgb565_palette[i] = hsv_to_rgb565(rgb_palette[i * 3 + 0], rgb_palette[i * 3 + 1], rgb_palette[i * 3 + 2]);
    }

    // Transmit each block of pixels
    stream_palette_pixdata_impl(surf, rgb565_palette, bits_per_pixel, pixel_count, pixel_data, byte_count);
}

// Recolored renderer
static inline void stream_mono_pixdata_recolor(qmk_rgb565_surface_device_t *surf, uint8_t bits_per_pixel, uint32_t pixel_count, const void *const pixel_data, uint32_t byte_count, int16_t hue_fg, int16_t sat_fg, int16_t val_fg, int16_t hue_bg, int16_t sat_bg, int16_t val_bg) {
    // Memoize the last batch of colours so we're not regenerating the palette if we're not changing anything
    static uint8_t last_bits_per_pixel = UINT8_MAX;
    static int16_t last_hue_fg         = INT16_MIN;
    static int16_t last_sat_fg         = INT16_MIN;
    static int16_t last_val_fg         = INT16_MIN;
    static int16_t last_hue_bg         = INT16_MIN;
    static int16_t last_sat_bg         = INT16_MIN;
    static int16_t last_val_bg         = INT16_MIN;

    // Regenerate the palette only if the inputs have changed
    if (last_bits_per_pixel != bits_per_pixel || last_hue_fg != hue_fg || last_sat_fg != sat_fg || last_val_fg != val_fg || last_hue_bg != hue_bg || last_sat_bg != sat_bg || last_val_bg != val_bg) {
        last_bits_per_pixel = bits_per_pixel;
        last_hue_fg         = hue_fg;
        last_sat_fg         = sat_fg;
        last_val_fg         = val_fg;
        last_hue_bg         = hue_bg;
        last_sat_bg         = sat_bg;
        last_val_bg         = val_bg;

        // Generate the color lookup table
        uint16_t items = 1 << bits_per_pixel;  // number of items we need to interpolate
        qp_interpolate_palette(hsv_lookup_table, items, (qp_pixel_color_t){.hsv888 = {.h = hue_fg, .s = sat_fg, .v = val_fg}}, (qp_pixel_color_t){.hsv888 = {.h = hue_bg, .s = sat_bg, .v = val_bg}});
        for (uint16_t i = 0; i < items; ++i) {
            rgb565_palette[i] = hsv_to_rgb565(hsv_lookup_table[i].hsv888.h, hsv_lookup_table[i].hsv888.s, hsv_lookup_table[i].hsv888.v);
        }
    }

    // Transmit each block of pixels
    stream_palette_pixdata_impl(surf, rgb565_palette, bits_per_pixel, pixel_count, pixel_data, byte_count);
}

// Uncompressed image drawing helper
static bool drawimage_uncompressed_impl(qmk_rgb565_surface_device_t *surf, painter_image_format_t image_format, uint8_t image_bpp, const uint8_t *pixel_data, uint32_t byte_count, int32_t width, int32_t height, const uint8_t *palette_data, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg) {
    // Stream data to the LCD
    if (image_format == IMAGE_FORMAT_RAW || image_format == IMAGE_FORMAT_RGB565) {
        // The pixel data is in the correct format already -- send it directly to the device
        stream_pixdata(surf, (const uint16_t *)pixel_data, width * height);
    } else if (image_format == IMAGE_FORMAT_GRAYSCALE) {
        // Supplied pixel data is in 4bpp monochrome -- decode it to the equivalent pixel data
        stream_mono_pixdata_recolor(surf, image_bpp, width * height, pixel_data, byte_count, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
    } else if (image_format == IMAGE_FORMAT_PALETTE) {
        // Supplied pixel data is in 1bpp monochrome -- decode it to the equivalent pixel data
        stream_palette_pixdata(surf, palette_data, image_bpp, width * height, pixel_data, byte_count);
    }

    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool qp_rgb565_surface_init(painter_device_t device, painter_rotation_t rotation) {
    (void)rotation;  // no rotation supported.
    qmk_rgb565_surface_device_t *surf = (qmk_rgb565_surface_device_t *)device;
    if (surf->buffer) {
        return true;
    }

    uint16_t *buffer = (uint16_t *)malloc(surf->width * surf->height * sizeof(uint16_t));
    if (!buffer) {
        return false;
    }

    surf->buffer = buffer;
    memset(surf->buffer, 0, surf->width * surf->height * sizeof(uint16_t));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool qp_rgb565_surface_clear(painter_device_t device) {
    qmk_rgb565_surface_device_t *surf = (qmk_rgb565_surface_device_t *)device;
    if (!surf->buffer) {
        return false;
    }

    memset(surf->buffer, 0, sizeof(surf->width * surf->height * sizeof(uint16_t)));
    return true;
}

bool qp_rgb565_surface_power(painter_device_t device, bool power_on) { return true; }

bool qp_rgb565_surface_pixdata(painter_device_t device, const void *pixel_data, uint32_t native_pixel_count) {
    qmk_rgb565_surface_device_t *surf = (qmk_rgb565_surface_device_t *)device;
    if (!surf->buffer) {
        return false;
    }

    const uint16_t *data = (const uint16_t *)pixel_data;
    stream_pixdata(surf, data, native_pixel_count);
    return true;
}

bool qp_rgb565_surface_viewport(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) {
    qmk_rgb565_surface_device_t *surf = (qmk_rgb565_surface_device_t *)device;
    if (!surf->buffer) {
        return false;
    }

    // Set the viewport locations
    surf->viewport_l = left;
    surf->viewport_t = top;
    surf->viewport_r = right;
    surf->viewport_b = bottom;

    // Reset the write location to the top left
    surf->pixdata_x = left;
    surf->pixdata_y = top;
    return true;
}

bool qp_rgb565_surface_setpixel(painter_device_t device, uint16_t x, uint16_t y, uint8_t hue, uint8_t sat, uint8_t val) {
    qmk_rgb565_surface_device_t *surf = (qmk_rgb565_surface_device_t *)device;
    if (!surf->buffer) {
        return false;
    }

    setpixel(surf, x, y, hsv_to_rgb565(hue, sat, val));
    return true;
}

// Draw an image
bool qp_rgb565_surface_drawimage(painter_device_t device, uint16_t x, uint16_t y, const painter_image_descriptor_t *image, uint8_t hue, uint8_t sat, uint8_t val) {
    qmk_rgb565_surface_device_t *surf = (qmk_rgb565_surface_device_t *)device;
    if (!surf->buffer) {
        return false;
    }

    // Configure where we're rendering to
    qp_rgb565_surface_viewport(device, x, y, x + image->width - 1, y + image->height - 1);

    bool ret = false;
    if (image->compression == IMAGE_UNCOMPRESSED) {
        const painter_raw_image_descriptor_t *raw_image_desc = (const painter_raw_image_descriptor_t *)image;
        ret                                                  = drawimage_uncompressed_impl(surf, image->image_format, image->image_bpp, raw_image_desc->image_data, raw_image_desc->byte_count, image->width, image->height, raw_image_desc->image_palette, hue, sat, val, hue, sat, 0);
    }

    return ret;
}

int16_t qp_rgb565_surface_drawtext(painter_device_t device, uint16_t x, uint16_t y, painter_font_t font, const char *str, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg) {
    qmk_rgb565_surface_device_t *surf = (qmk_rgb565_surface_device_t *)device;
    if (!surf->buffer) {
        return false;
    }

    const painter_raw_font_descriptor_t *fdesc = (const painter_raw_font_descriptor_t *)font;

    const char *c = str;
    while (*c) {
        int32_t code_point = 0;
        c                  = decode_utf8(c, &code_point);

        if (code_point >= 0) {
            if (code_point >= 0x20 && code_point < 0x7F) {
                if (fdesc->ascii_glyph_definitions != NULL) {
                    // Search the font's ascii table
                    uint8_t                                  index      = code_point - 0x20;
                    const painter_font_ascii_glyph_offset_t *glyph_desc = &fdesc->ascii_glyph_definitions[index];
                    uint16_t                                 byte_count = 0;
                    if (code_point < 0x7E) {
                        byte_count = (glyph_desc + 1)->offset - glyph_desc->offset;
                    } else if (code_point == 0x7E) {
#ifdef UNICODE_ENABLE
                        // Unicode glyphs directly follow ascii glyphs, so we take the first's offset
                        if (fdesc->unicode_glyph_count > 0) {
                            byte_count = fdesc->unicode_glyph_definitions[0].offset - glyph_desc->offset;
                        } else {
                            byte_count = fdesc->byte_count - glyph_desc->offset;
                        }
#else   // UNICODE_ENABLE
                        byte_count = fdesc->byte_count - glyph_desc->offset;
#endif  // UNICODE_ENABLE
                    }

                    qp_rgb565_surface_viewport(surf, x, y, x + glyph_desc->width - 1, y + font->glyph_height - 1);
                    drawimage_uncompressed_impl(surf, font->image_format, font->image_bpp, &fdesc->image_data[glyph_desc->offset], byte_count, glyph_desc->width, font->glyph_height, fdesc->image_palette, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
                    x += glyph_desc->width;
                }
            }
#ifdef UNICODE_ENABLE
            else {
                // Search the font's unicode table
                if (fdesc->unicode_glyph_definitions != NULL) {
                    for (uint16_t index = 0; index < fdesc->unicode_glyph_count; ++index) {
                        const painter_font_unicode_glyph_offset_t *glyph_desc = &fdesc->unicode_glyph_definitions[index];
                        if (glyph_desc->unicode_glyph == code_point) {
                            uint16_t byte_count = (index == fdesc->unicode_glyph_count - 1) ? (fdesc->byte_count - glyph_desc->offset) : ((glyph_desc + 1)->offset - glyph_desc->offset);
                            qp_rgb565_surface_viewport(surf, x, y, x + glyph_desc->width - 1, y + font->glyph_height - 1);
                            drawimage_uncompressed_impl(surf, font->image_format, font->image_bpp, &fdesc->image_data[glyph_desc->offset], byte_count, glyph_desc->width, font->glyph_height, fdesc->image_palette, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
                            x += glyph_desc->width;
                        }
                    }
                }
            }
#endif  // UNICODE_ENABLE
        }
    }

    return (int16_t)x;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Device creation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Driver vtable
static const struct painter_driver_vtable_t QP_RESIDENT_FLASH driver_vtable = {
    .init      = qp_rgb565_surface_init,
    .clear     = qp_rgb565_surface_clear,
    .power     = qp_rgb565_surface_power,
    .pixdata   = qp_rgb565_surface_pixdata,
    .viewport  = qp_rgb565_surface_viewport,
    .setpixel  = qp_rgb565_surface_setpixel,
    .line      = qp_fallback_line,
    .rect      = qp_fallback_rect,
    .circle    = qp_fallback_circle,
    .ellipse   = qp_fallback_ellipse,
    .drawimage = qp_rgb565_surface_drawimage,
    .drawtext  = qp_rgb565_surface_drawtext,
};

// Driver storage
static qmk_rgb565_surface_device_t driver = {0};

// Factory function for creating a handle to the ILI9341 device
painter_device_t qp_rgb565_surface_make_device(uint16_t width, uint16_t height) {
    // Skip creation if we've got a zero-pixel width or height
    if (width == 0 || height == 0) {
        return NULL;
    }

    // Skip adding a new surface if we've already allocated it.
    if (driver.width != 0 || driver.height != 0) {
        return NULL;
    }

    driver.qp_driver.driver_vtable = &driver_vtable;
    driver.width                   = width;
    driver.height                  = height;
    return (painter_device_t)&driver;
}

const void *qp_rgb565_surface_get_buffer_ptr(painter_device_t device) {
    qmk_rgb565_surface_device_t *surf = (qmk_rgb565_surface_device_t *)device;
    return surf->buffer;
}

uint32_t qp_rgb565_surface_get_pixel_count(painter_device_t device) {
    qmk_rgb565_surface_device_t *surf = (qmk_rgb565_surface_device_t *)device;
    return ((uint32_t)surf->width) * surf->height;
}