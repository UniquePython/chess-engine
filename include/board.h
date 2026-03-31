#ifndef BOARD_H
#define BOARD_H

#include "piece.h"

#include <stddef.h>

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

typedef struct board
{
    Piece squares[64];
} Board;

/* core API */
void init_board(Board *b);

Piece get(Board *b, Loc l);
void set(Board *b, Loc l, Piece p);

size_t index_of(Loc l);
Loc loc_of(size_t i);

void print_board(Board *b);

#endif