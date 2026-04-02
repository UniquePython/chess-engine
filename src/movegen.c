#include "movegen.h"

#include <stdbool.h>

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

static void generate_pawn_moves(Board *b, Loc from, Move *moves, int *count)
{
    Piece p = get(b, from);

    if (p.colour == WHITE)
        gen_white_pawn(b, from, moves, count);
    else if (p.colour == BLACK)
        gen_black_pawn(b, from, moves, count);
}

static void generate_knight_moves(Board *b, Loc from, Move *moves, int *count)
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

static void generate_bishop_moves(Board *b, Loc from, Move *moves, int *count)
{
    int dirs[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    gen_sliding(b, from, moves, count, dirs, 4);
}

static void generate_rook_moves(Board *b, Loc from, Move *moves, int *count)
{
    int dirs[4][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
    gen_sliding(b, from, moves, count, dirs, 4);
}

static void generate_queen_moves(Board *b, Loc from, Move *moves, int *count)
{
    generate_bishop_moves(b, from, moves, count);
    generate_rook_moves(b, from, moves, count);
}

static void generate_king_moves(Board *b, Loc from, Move *moves, int *count)
{
    int dirs[8][2] = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};

    Piece p = get(b, from);

    for (int i = 0; i < 8; i++)
    {
        int r = from.rank + dirs[i][0];
        int f = from.file + dirs[i][1];

        // bounds check
        if (ONE > r || r > EIGHT || A > f || f > H)
            continue;

        Loc to = {r, f};
        Piece target = get(b, to);

        // not friendly → add move
        if (!is_same_color(p, target))
            add_move(moves, count, from, to);
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

        case BISHOP:
            generate_bishop_moves(b, from, moves, &count);
            break;

        case ROOK:
            generate_rook_moves(b, from, moves, &count);
            break;

        case QUEEN:
            generate_queen_moves(b, from, moves, &count);
            break;

        case KING:
            generate_king_moves(b, from, moves, &count);
            break;

        default:
            break;
        }
    }

    return count;
}

bool is_square_attacked(Board *b, Loc square, Side attacker)
{
    PC attacking_colour = (attacker == SIDE_WHITE) ? WHITE : BLACK;

    // rook/queen (cardinal rays)
    int cardinal[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (int d = 0; d < 4; d++)
    {
        int r = square.rank;
        int f = square.file;
        while (1)
        {
            r += cardinal[d][0];
            f += cardinal[d][1];
            if (r < ONE || r > EIGHT || f < A || f > H)
                break;
            Piece p = get(b, (Loc){r, f});
            if (!is_empty(p))
            {
                if (p.colour == attacking_colour && (p.type == ROOK || p.type == QUEEN))
                    return true;
                break;
            }
        }
    }

    // bishop/queen (diagonal rays)
    int diagonal[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (int d = 0; d < 4; d++)
    {
        int r = square.rank;
        int f = square.file;
        while (1)
        {
            r += diagonal[d][0];
            f += diagonal[d][1];
            if (r < ONE || r > EIGHT || f < A || f > H)
                break;
            Piece p = get(b, (Loc){r, f});
            if (!is_empty(p))
            {
                if (p.colour == attacking_colour && (p.type == BISHOP || p.type == QUEEN))
                    return true;
                break;
            }
        }
    }

    // knight
    int knight_jumps[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
    for (int i = 0; i < 8; i++)
    {
        int r = square.rank + knight_jumps[i][0];
        int f = square.file + knight_jumps[i][1];
        if (r < ONE || r > EIGHT || f < A || f > H)
            continue;
        Piece p = get(b, (Loc){r, f});
        if (p.colour == attacking_colour && p.type == KNIGHT)
            return true;
    }

    // pawns
    int pawn_rank_dir = (attacker == SIDE_WHITE) ? -1 : 1;
    int pawn_files[2] = {-1, 1};
    for (int i = 0; i < 2; i++)
    {
        int r = square.rank + pawn_rank_dir;
        int f = square.file + pawn_files[i];
        if (r < ONE || r > EIGHT || f < A || f > H)
            continue;
        Piece p = get(b, (Loc){r, f});
        if (p.colour == attacking_colour && p.type == PAWN)
            return true;
    }

    // king
    int king_dirs[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (int i = 0; i < 8; i++)
    {
        int r = square.rank + king_dirs[i][0];
        int f = square.file + king_dirs[i][1];
        if (r < ONE || r > EIGHT || f < A || f > H)
            continue;
        Piece p = get(b, (Loc){r, f});
        if (p.colour == attacking_colour && p.type == KING)
            return true;
    }

    return false;
}

bool is_in_check(Game *g, Side side)
{
    PC colour = (side == SIDE_WHITE) ? WHITE : BLACK;
    Side attacker = (side == SIDE_WHITE) ? SIDE_BLACK : SIDE_WHITE;

    // find the king
    for (int i = 0; i < 64; i++)
    {
        Piece p = get(&g->board, loc_of(i));
        if (p.colour == colour && p.type == KING)
            return is_square_attacked(&g->board, loc_of(i), attacker);
    }

    return false; // no king found (shouldn't happen in a valid game)
}