#include "movegen.h"

static void gen_white_pawn(Board *b, Loc from, Move *moves, int *count)
{
    Loc one_forward = {from.rank + 1, from.file};

    // 1 step forward
    if (is_empty(get(b, one_forward)))
    {
        add_move(moves, count, from, one_forward);

        // 2 step forward (only from rank TWO)
        if (from.rank == TWO)
        {
            Loc two_forward = {from.rank + 2, from.file};

            if (is_empty(get(b, two_forward)))
                add_move(moves, count, from, two_forward);
        }
    }

    // captures
    if (from.file > A)
    {
        Loc left_cap = {from.rank + 1, from.file - 1};
        Piece target = get(b, left_cap);

        if (!is_empty(target) && is_black(target))
            add_move(moves, count, from, left_cap);
    }

    if (from.file < H)
    {
        Loc right_cap = {from.rank + 1, from.file + 1};
        Piece target = get(b, right_cap);

        if (!is_empty(target) && is_black(target))
            add_move(moves, count, from, right_cap);
    }
}

static void gen_black_pawn(Board *b, Loc from, Move *moves, int *count)
{
    Loc one_forward = {from.rank - 1, from.file};

    // 1 step forward
    if (is_empty(get(b, one_forward)))
    {
        add_move(moves, count, from, one_forward);

        // 2 step forward (only from rank SEVEN)
        if (from.rank == SEVEN)
        {
            Loc two_forward = {from.rank - 2, from.file};

            if (is_empty(get(b, two_forward)))
                add_move(moves, count, from, two_forward);
        }
    }

    // captures
    if (from.file > A)
    {
        Loc left_cap = {from.rank - 1, from.file - 1};
        Piece target = get(b, left_cap);

        if (!is_empty(target) && is_white(target))
            add_move(moves, count, from, left_cap);
    }

    if (from.file < H)
    {
        Loc right_cap = {from.rank - 1, from.file + 1};
        Piece target = get(b, right_cap);

        if (!is_empty(target) && is_white(target))
            add_move(moves, count, from, right_cap);
    }
}

void generate_pawn_moves(Board *b, Loc from, Move *moves, int *count)
{
    Piece p = get(b, from);

    if (p.type != PAWN)
        return;

    if (p.colour == WHITE)
        gen_white_pawn(b, from, moves, count);
    else if (p.colour == BLACK)
        gen_black_pawn(b, from, moves, count);
}

int generate_moves(Board *b, Side side, Move *moves)
{
    int count = 0;

    for (int i = 0; i < 64; i++)
    {
        Loc from = loc_of(i);
        Piece p = get(b, from);

        if (is_empty(p))
            continue;

        // filter by side
        if (side == SIDE_WHITE && p.colour != WHITE)
            continue;

        if (side == SIDE_BLACK && p.colour != BLACK)
            continue;

        // dispatch by piece type
        switch (p.type)
        {
        case PAWN:
            generate_pawn_moves(b, from, moves, &count);
            break;

        default:
            break; // other pieces later
        }
    }

    return count;
}