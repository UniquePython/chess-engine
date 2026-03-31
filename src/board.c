#include "board.h"

#include <stdio.h>

/* indexing */
size_t index_of(Loc l)
{
    return l.rank * 8 + l.file;
}

Loc loc_of(size_t i)
{
    return (Loc){i / 8, i % 8};
}

/* access */
Piece get(Board *b, Loc l)
{
    return b->squares[index_of(l)];
}

void set(Board *b, Loc l, Piece p)
{
    b->squares[index_of(l)] = p;
}

/* init */
void init_board(Board *b)
{
    for (int i = 0; i < 64; i++)
        b->squares[i] = no_piece();

    for (File f = A; f <= H; f++)
    {
        set(b, (Loc){TWO, f}, white(PAWN));
        set(b, (Loc){SEVEN, f}, black(PAWN));
    }

    set(b, (Loc){ONE, A}, white(ROOK));
    set(b, (Loc){ONE, B}, white(KNIGHT));
    set(b, (Loc){ONE, C}, white(BISHOP));
    set(b, (Loc){ONE, D}, white(QUEEN));
    set(b, (Loc){ONE, E}, white(KING));
    set(b, (Loc){ONE, F}, white(BISHOP));
    set(b, (Loc){ONE, G}, white(KNIGHT));
    set(b, (Loc){ONE, H}, white(ROOK));

    set(b, (Loc){EIGHT, A}, black(ROOK));
    set(b, (Loc){EIGHT, B}, black(KNIGHT));
    set(b, (Loc){EIGHT, C}, black(BISHOP));
    set(b, (Loc){EIGHT, D}, black(QUEEN));
    set(b, (Loc){EIGHT, E}, black(KING));
    set(b, (Loc){EIGHT, F}, black(BISHOP));
    set(b, (Loc){EIGHT, G}, black(KNIGHT));
    set(b, (Loc){EIGHT, H}, black(ROOK));
}

/* printing */
void print_board(Board *b)
{
    for (int r = EIGHT; r >= ONE; r--)
    {
        printf("%d  ", r + 1);

        for (int f = A; f <= H; f++)
        {
            printf("%c ", piece_to_char(get(b, (Loc){r, f})));
        }

        printf("\n");
    }

    printf("\n   a b c d e f g h\n");
}