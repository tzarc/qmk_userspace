// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define GEN_QUEUE_NUM_ENTRIES 16
#define GEN_QUEUE_VALUE_TYPE char
#define GEN_QUEUE_NAMING_PREFIX
#include "gen_queue.h"

#define GEN_QUEUE_NUM_ENTRIES 8
#define GEN_QUEUE_VALUE_TYPE int
#define GEN_QUEUE_NAMING_PREFIX event_
#define GEN_QUEUE_ATTRIBUTE __attribute__((align(2)))
#define GEN_QUEUE_DATA_ATTRIBUTE __attribute__((align(2)))
#define GEN_QUEUE_EXTRA_FIELDS \
    int lock_count;            \
    int unlock_count;          \
    int level;
#include "gen_queue.h"

static event_queue_t queue;
static queue_t       q;

void print_lock_stats(event_queue_t *queue) {
    printf("     Locks: %d, Unlocks; %d, Level: %d\n", queue->lock_count, queue->unlock_count, queue->level);
}

void event_queue_lock(event_queue_t *queue) {
    ++queue->lock_count;
    ++queue->level;
    print_lock_stats(queue);
}

void event_queue_unlock(event_queue_t *queue) {
    ++queue->unlock_count;
    --queue->level;
    print_lock_stats(queue);
}

int main(int argc, const char *argv[]) {
    event_queue_init(&queue);
    queue_init(&q);

    for (int i = 0; i < 16; ++i) {
        if (!event_queue_push(&queue, &i)) {
            while (!event_queue_empty(&queue)) {
                int dummy;
                event_queue_pop(&queue, &dummy);
                printf("< Pop: %d\n", (int)dummy);
            }
            event_queue_push(&queue, &i);
            printf("> Push: %d\n", i);
        } else {
            printf("> Push: %d\n", i);
        }
    }

    return 0;
}