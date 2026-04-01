#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "movegen.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct undo_information
{
    Move move_to_undo;

    Piece captured_piece;

    bool white_king_can_castle_kingside;
    bool white_king_can_castle_queenside;
    bool black_king_can_castle_kingside;
    bool black_king_can_castle_queenside;

    bool en_passant_available;
    Loc en_passant_square;
} UndoInfo;

typedef struct game
{
    Board board;
    Side current_side;

    bool white_king_can_castle_kingside;
    bool white_king_can_castle_queenside;
    bool black_king_can_castle_kingside;
    bool black_king_can_castle_queenside;

    bool en_passant_available;
    Loc en_passant_square;

    UndoInfo history[6000];
    size_t n_moves;
} Game;

void init_game(Game *game);

void apply_move(Game *g, Move m);

#endif