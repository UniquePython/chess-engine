#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "move.h"

#include <stdbool.h>
#include <stddef.h>

typedef enum
{
    GAME_ONGOING,
    GAME_CHECKMATE,
    GAME_STALEMATE
} GameStatus;

typedef enum
{
    SIDE_WHITE,
    SIDE_BLACK
} Side;

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
void undo_move(Game *g);

#endif