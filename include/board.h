#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "loc.h"

typedef struct board
{
    Piece squares[64];
} Board;

/* core API */
void init_board(Board *b);

Piece get(Board *b, Loc l);
void set(Board *b, Loc l, Piece p);

void print_board(Board *b);

#endif