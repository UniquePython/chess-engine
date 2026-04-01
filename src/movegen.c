#include "movegen.h"

static void gen_white_pawn(Board *b, Loc from, Move *moves, int *count)
{
    if (from.rank == EIGHT)
        return;

    Loc one_forward = {from.rank + 1, from.file};

    // 1 step forward
    if (is_empty(get(b, one_forward)))
    {
        if (one_forward.rank == EIGHT)
            add_promotion(moves, count, from, one_forward);
        else
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
        {
            if (left_cap.rank == EIGHT)
                add_promotion(moves, count, from, left_cap);
            else
                add_move(moves, count, from, left_cap);
        }
    }

    if (from.file < H)
    {
        Loc right_cap = {from.rank + 1, from.file + 1};
        Piece target = get(b, right_cap);

        if (!is_empty(target) && is_black(target))
        {
            if (right_cap.rank == EIGHT)
                add_promotion(moves, count, from, right_cap);
            else
                add_move(moves, count, from, right_cap);
        }
    }
}

static void gen_black_pawn(Board *b, Loc from, Move *moves, int *count)
{
    if (from.rank == ONE)
        return;

    Loc one_forward = {from.rank - 1, from.file};

    // 1 step forward
    if (is_empty(get(b, one_forward)))
    {
        if (one_forward.rank == ONE)
            add_promotion(moves, count, from, one_forward);
        else
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
        {
            if (left_cap.rank == ONE)
                add_promotion(moves, count, from, left_cap);
            else
                add_move(moves, count, from, left_cap);
        }
    }

    if (from.file < H)
    {
        Loc right_cap = {from.rank - 1, from.file + 1};
        Piece target = get(b, right_cap);

        if (!is_empty(target) && is_white(target))
        {
            if (right_cap.rank == ONE)
                add_promotion(moves, count, from, right_cap);
            else
                add_move(moves, count, from, right_cap);
        }
    }
}

void generate_pawn_moves(Board *b, Loc from, Move *moves, int *count)
{
    Piece p = get(b, from);

    if (p.colour == WHITE)
        gen_white_pawn(b, from, moves, count);
    else if (p.colour == BLACK)
        gen_black_pawn(b, from, moves, count);
}

void generate_knight_moves(Board *b, Loc from, Move *moves, int *count)
{
    int offsets[8][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};

    Piece p = get(b, from);

    for (int i = 0; i < 8; i++)
    {
        int r = from.rank + offsets[i][0];
        int f = from.file + offsets[i][1];

        // bounds check
        if (ONE > r || r > EIGHT || A > f || f > H)
            continue;

        // get target, check not friendly
        Loc jump = {r, f};
        Piece target = get(b, jump);

        if (is_same_color(p, target))
            continue;

        add_move(moves, count, from, jump);
    }
}

static void gen_sliding(Board *b, Loc from, Move *moves, int *count, int dirs[][2], int ndir)
{
    Piece p = get(b, from);

    for (int d = 0; d < ndir; d++)
    {
        int r = from.rank;
        int f = from.file;

        while (1)
        {
            r += dirs[d][0];
            f += dirs[d][1];

            // bounds check
            if (ONE > r || r > EIGHT || A > f || f > H)
                break;

            Loc to = {r, f};
            Piece target = get(b, to);

            // friendly → break
            if (is_same_color(p, target))
                break;

            // enemy → break (capture, but stop sliding)
            if (is_opposite_color(p, target))
            {
                add_move(moves, count, from, to);
                break;
            }

            add_move(moves, count, from, to);
        }
    }
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

        case KNIGHT:
            generate_knight_moves(b, from, moves, &count);
            break;

        default:
            break; // other pieces later
        }
    }

    return count;
}