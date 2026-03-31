#ifndef MOVE_H
#define MOVE_H

#include "board.h"
#include <stdbool.h>

typedef struct
{
    Loc from;
    Loc to;

    PT promotion; // NO_PIECE if not a promotion

    bool is_castle;
    bool is_en_passant;
} Move;

#endif