#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "move.h"
#include "board.h"
#include "game.h"

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

static inline void add_promotion(Move *moves, int *count, Loc from, Loc to)
{
    PT types[4] = {QUEEN, ROOK, BISHOP, KNIGHT};

    for (int i = 0; i < 4; i++)
    {
        moves[*count] = (Move){
            .from = from,
            .to = to,
            .promotion = types[i],
            .is_castle = false,
            .is_en_passant = false};
        (*count)++;
    }
}

int generate_moves(Board *b, Side side, Move *moves);

bool is_square_attacked(Board *b, Loc square, Side attacker);
bool is_in_check(Game *g, Side side);

#endif