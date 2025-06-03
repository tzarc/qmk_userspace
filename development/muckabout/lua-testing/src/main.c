// Copyright 2025 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <onelua.c>

typedef struct tracking_t {
    size_t allocated;
} tracking_t;

static void *l_alloc_tracked(void *ud, void *ptr, size_t osize, size_t nsize) {
    static size_t allocated     = 0;
    static size_t max_allocated = 0;

    if (nsize == 0) {
        if (ptr) {
            printf("l_alloc_tracked:(opaque_ptr=%p) (ptr=%p) (osize=%zu) (nsize=%zu) (total=%zu) (max=%zu)\n", ud, ptr, osize, nsize, allocated, max_allocated);
            tracking_t *tracking = ((tracking_t *)ptr) - 1;
            allocated -= tracking->allocated;
            free(tracking);
        }
        printf("l_alloc_tracked:(opaque_ptr=%p) (ptr=%p) (osize=%zu) (nsize=%zu) (total=%zu) (max=%zu)\n", ud, ptr, osize, nsize, allocated, max_allocated);
        return NULL;
    } else {
        printf("l_alloc_tracked:(opaque_ptr=%p) (ptr=%p) (osize=%zu) (nsize=%zu) (total=%zu) (max=%zu)\n", ud, ptr, osize, nsize, allocated, max_allocated);
        if (ptr) {
            tracking_t *tracking = ((tracking_t *)ptr) - 1;
            allocated -= tracking->allocated;
            tracking->allocated = nsize;
            allocated += nsize;
            if (allocated > max_allocated) {
                max_allocated = allocated;
            }
            return ((tracking_t *)realloc(tracking, sizeof(tracking_t) + nsize)) + 1;
        } else {
            tracking_t *tracking = calloc(sizeof(tracking_t) + nsize, 1);
            tracking->allocated  = nsize;
            allocated += nsize;
            if (allocated > max_allocated) {
                max_allocated = allocated;
            }
            return tracking + 1;
        }
    }
}

static lua_State *luaL_newstate_tracked(void) {
    lua_State *L = lua_newstate(l_alloc_tracked, NULL);
    if (l_likely(L)) {
        lua_atpanic(L, &panic);
        lua_setwarnf(L, warnfoff, L); /* default is warnings off */
    }
    return L;
}

#include "keycode_strings.h"

int main() {
    lua_State *L = luaL_newstate_tracked();
    printf("---\n");
    lua_pushstring(L, "");
    int i = 0;
    for (const struct keycode_t *p = keycodes; p->name; p++) {
        printf("%s\n", p->name);
        lua_pushnumber(L, p->value);
        lua_setglobal(L, p->name);
    }
    lua_close(L);
    return 0;
}
