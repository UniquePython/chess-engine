#ifndef MOVE_H
#define MOVE_H

#include "board.h"
#include <stdbool.h>

typedef enum rank
{
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT
} Rank;

typedef enum file
{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H
} File;

typedef struct location
{
    Rank rank;
    File file;
} Loc;

typedef struct
{
    Loc from;
    Loc to;

    PT promotion; // NO_PIECE if not a promotion

    bool is_castle;
    bool is_en_passant;
} Move;

static inline size_t index_of(Loc l)
{
    return l.rank * 8 + l.file;
}

static inline Loc loc_of(size_t i)
{
    return (Loc){i / 8, i % 8};
}

#endif