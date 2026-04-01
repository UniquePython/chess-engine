#include "game.h"

#include <stdlib.h>

void init_game(Game *game)
{
    init_board(&game->board);

    game->current_side = SIDE_WHITE;

    game->white_king_can_castle_kingside = true;
    game->white_king_can_castle_queenside = true;
    game->black_king_can_castle_kingside = true;
    game->black_king_can_castle_queenside = true;

    game->en_passant_available = false;
    game->en_passant_square = (Loc){NO_RANK, NO_FILE};

    game->n_moves = 0;

    return game;
}