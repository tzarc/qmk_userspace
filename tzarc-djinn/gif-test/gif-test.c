#include <quantum.h>
#include <qp.h>
#include "gif.h"
#include "badgers.h"

extern painter_device_t lcd;

bool gif_loaded = false;
GIFIMAGE gif;

typedef struct _render_loc_t {
    uint16_t x, y;
} render_loc_t;

void draw_callback(GIFDRAW* pDraw, GIFIMAGE* pImage, void* pExtData) {
    render_loc_t *loc = (render_loc_t*)pExtData;

    uint16_t l = loc->x + pDraw->iX;
    uint16_t t = loc->y + pDraw->iY + pDraw->y;
    uint16_t r = l + pDraw->iWidth - 1;
    uint16_t b = t; // we're only drawing one line at a time

    uint8_t *p = pDraw->pPixels;
    uint16_t buf[320];
    for(int x = 0; x < pDraw->iWidth; ++x) {
        buf[x] = pDraw->pPalette[*p++];
    }

    qp_viewport(lcd, l, t, r, b);
    qp_pixdata(lcd, buf, pDraw->iWidth);
}

void keyboard_post_init_user_gifrender(void) {
    gif_loaded = GIF_openRAM(&gif, ucBadgers, sizeof(ucBadgers), &draw_callback);
}

void housekeeping_task_user_gifrender(void) {
    if(!gif_loaded) return;

    static uint32_t next_frame = 0;

    render_loc_t loc = { .x = 16, .y = 64 };

    uint32_t now = timer_read32();
    if(now >= next_frame) {
        static int last_frame_delay = 0;
        int frame_delay;
        bool more_frames = GIF_playFrame(&gif, &frame_delay, &loc);
        if(more_frames) {
            last_frame_delay = frame_delay;
        }
        next_frame = timer_read32() + last_frame_delay;
    }
}