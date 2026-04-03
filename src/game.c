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
}

void apply_move(Game *g, Move m)
{
    UndoInfo record = {
        .move_to_undo = m,
        .captured_piece = get(&g->board, m.to),
        .white_king_can_castle_kingside = g->white_king_can_castle_kingside,
        .white_king_can_castle_queenside = g->white_king_can_castle_queenside,
        .black_king_can_castle_kingside = g->black_king_can_castle_kingside,
        .black_king_can_castle_queenside = g->black_king_can_castle_queenside,
        .en_passant_available = g->en_passant_available,
        .en_passant_square = g->en_passant_square,
    };

    Piece moving = get(&g->board, m.from);

    // handle special cases first
    if (m.is_en_passant)
    {
        Loc captured_pawn = {m.from.rank, m.to.file};
        record.captured_piece = get(&g->board, captured_pawn);
        set(&g->board, captured_pawn, no_piece());
    }

    if (m.is_castle)
    {
        // move king
        set(&g->board, m.to, moving);
        set(&g->board, m.from, no_piece());

        // move rook — determine from m.to
        // kingside: king lands on G, rook goes from H to F
        // queenside: king lands on C, rook goes from A to D
        Rank back_rank = (g->current_side == SIDE_WHITE) ? ONE : EIGHT;

        if (m.to.file == G)
        {
            set(&g->board, (Loc){back_rank, H}, no_piece());
            set(&g->board, (Loc){back_rank, F}, (g->current_side == SIDE_WHITE) ? white(ROOK) : black(ROOK));
        }
        else // queenside
        {
            set(&g->board, (Loc){back_rank, A}, no_piece());
            set(&g->board, (Loc){back_rank, D}, (g->current_side == SIDE_WHITE) ? white(ROOK) : black(ROOK));
        }
    }
    else
    {
        // normal move + promotion
        set(&g->board, m.to, (m.promotion != NO_PIECE) ? make_piece(moving.colour, m.promotion) : moving);
        set(&g->board, m.from, no_piece());
    }

    // update en passant state
    // was this a double pawn push? set en_passant_available + en_passant_square
    // otherwise clear it
    if (moving.type == PAWN && abs(m.to.rank - m.from.rank) == 2)
    {
        g->en_passant_available = true;
        g->en_passant_square = (Loc){(m.from.rank + m.to.rank) / 2, m.from.file};
    }
    else
    {
        g->en_passant_available = false;
        g->en_passant_square = (Loc){NO_RANK, NO_FILE};
    }

    // update castling rights
    // if king moved, lose both rights for that side
    // if rook moved, lose the relevant right
    if (moving.type == KING)
    {
        if (g->current_side == SIDE_WHITE)
        {
            g->white_king_can_castle_kingside = false;
            g->white_king_can_castle_queenside = false;
        }
        else
        {
            g->black_king_can_castle_kingside = false;
            g->black_king_can_castle_queenside = false;
        }
    }

    if (moving.type == ROOK)
    {
        if (m.from.file == A)
            g->current_side == SIDE_WHITE ? (g->white_king_can_castle_queenside = false)
                                          : (g->black_king_can_castle_queenside = false);
        if (m.from.file == H)
            g->current_side == SIDE_WHITE ? (g->white_king_can_castle_kingside = false)
                                          : (g->black_king_can_castle_kingside = false);
    }

    // flip side
    g->current_side = g->current_side == SIDE_WHITE ? SIDE_BLACK : SIDE_WHITE;

    // add to history
    g->history[g->n_moves++] = record;
}

void undo_move(Game *g)
{
    // pop the last record
    UndoInfo record = g->history[--g->n_moves];
    Move m = record.move_to_undo;

    // flip side back
    g->current_side = g->current_side == SIDE_WHITE ? SIDE_BLACK : SIDE_WHITE;

    // restore castling rights from record
    g->white_king_can_castle_kingside = record.white_king_can_castle_kingside;
    g->white_king_can_castle_queenside = record.white_king_can_castle_queenside;
    g->black_king_can_castle_kingside = record.black_king_can_castle_kingside;
    g->black_king_can_castle_queenside = record.black_king_can_castle_queenside;

    // restore en passant state from record
    g->en_passant_available = record.en_passant_available;
    g->en_passant_square = record.en_passant_square;

    // reverse the board changes
    Piece moved = get(&g->board, m.to);

    if (m.is_castle)
    {
        // move king back
        set(&g->board, m.from, moved);
        set(&g->board, m.to, no_piece());

        // move rook back (opposite of what apply_move did)
        Rank back_rank = (g->current_side == SIDE_WHITE) ? ONE : EIGHT;

        if (m.to.file == G) // kingside
        {
            set(&g->board, (Loc){back_rank, H}, (g->current_side == SIDE_WHITE) ? white(ROOK) : black(ROOK));
            set(&g->board, (Loc){back_rank, F}, no_piece());
        }
        else // queenside
        {
            set(&g->board, (Loc){back_rank, A}, (g->current_side == SIDE_WHITE) ? white(ROOK) : black(ROOK));
            set(&g->board, (Loc){back_rank, D}, no_piece());
        }
    }
    else
    {
        // move piece back to m.from
        // if promotion, restore a pawn instead of the promoted piece
        Piece original = m.promotion != NO_PIECE ? make_piece(moved.colour, PAWN) : moved;

        set(&g->board, m.from, original);

        // restore captured piece to m.to
        set(&g->board, m.to, record.captured_piece); // empty if no capture
    }

    if (m.is_en_passant)
    {
        // restore the captured pawn from record.captured_piece
        Loc captured_pawn_loc = {m.from.rank, m.to.file};
        set(&g->board, captured_pawn_loc, record.captured_piece);
    }
}
