#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "move.h"
#include "board.h"

typedef enum
{
    SIDE_WHITE,
    SIDE_BLACK
} Side;

static inline void add_move(Move *moves, int *count, Loc from, Loc to)
{
    moves[*count] = (Move){
        .from = from,
        .to = to,
        .promotion = NO_PIECE,
        .is_castle = false,
        .is_en_passant = false};

    (*count)++;
}

void generate_pawn_moves(Board *b, Loc from, Move *moves, int *count);
int generate_moves(Board *b, Side side, Move *moves);

#endif