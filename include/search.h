#ifndef SEARCH_H
#define SEARCH_H

#define _POSIX_C_SOURCE 199309L

#include "game.h"
#include "move.h"

#include <time.h>

typedef struct
{
    double allocated_ms;   // how long we're allowed to think
    struct timespec start; // when we started
} SearchTime;

Move search(Game *g, int depth, SearchTime *st);

static inline double elapsed_ms(SearchTime *st)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_sec - st->start.tv_sec) * 1000.0 + (now.tv_nsec - st->start.tv_nsec) / 1e6;
}

static inline bool time_up(SearchTime *st)
{
    return elapsed_ms(st) >= st->allocated_ms;
}

#endif