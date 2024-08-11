#include <stdint.h>

extern void effect_init(void *params) __attribute__((weak));
extern void effect_begin_iter(void *params, uint8_t led_min, uint8_t led_max) __attribute__((weak));
extern void effect_led(void *params, uint8_t led_index) __attribute__((weak));
extern void effect_end_iter(void *params) __attribute__((weak));

void effect_init_thunk(void) {
    if (!effect_init) return;
    register uintptr_t a0 asm("a0");
    effect_init((void *)a0);
}

void effect_begin_iter_thunk(void) {
    if (!effect_begin_iter) return;
    register uintptr_t a0 asm("a0");
    register uintptr_t a1 asm("a1");
    register uintptr_t a2 asm("a2");
    effect_begin_iter((void *)a0, (uint8_t)a1, (uint8_t)a2);
}

void effect_led_thunk(void) {
    if (!effect_led) return;
    register uintptr_t a0 asm("a0");
    register uintptr_t a1 asm("a1");
    effect_led((void *)a0, (uint8_t)a1);
}

void effect_end_iter_thunk(void) {
    if (!effect_end_iter) return;
    register uintptr_t a0 asm("a0");
    effect_end_iter((void *)a0);
}
