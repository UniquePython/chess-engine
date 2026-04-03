#include "game.h"
#include "movegen.h"
#include "search.h"

#include <stdio.h>
#include <stdlib.h>

#define SEARCH_DEPTH 7

static void print_move(Move m)
{
    char files[] = "abcdefgh";
    char ranks[] = "12345678";
    char promo[] = ".pnbrqk";

    printf("%c%c -> %c%c",
           files[m.from.file], ranks[m.from.rank],
           files[m.to.file], ranks[m.to.rank]);

    if (m.promotion != NO_PIECE)
        printf(" =%c", promo[m.promotion]);
    if (m.is_castle)
        printf(" (castle)");
    if (m.is_en_passant)
        printf(" (en passant)");

    printf("\n");
}

int main(void)
{
    Game g;
    init_game(&g);

    printf("Chess Engine\n");
    printf("Depth: %d\n\n", SEARCH_DEPTH);

    while (1)
    {
        print_board(&g.board);
        printf("\n");

        GameStatus status = get_game_status(&g);

        if (status == GAME_CHECKMATE)
        {
            printf("%s is checkmated. %s wins!\n",
                   g.current_side == SIDE_WHITE ? "White" : "Black",
                   g.current_side == SIDE_WHITE ? "Black" : "White");
            break;
        }

        if (status == GAME_STALEMATE)
        {
            printf("Stalemate. Draw!\n");
            break;
        }

        printf("%s to move. Searching...\n",
               g.current_side == SIDE_WHITE ? "White" : "Black");

        Move best = search(&g, SEARCH_DEPTH);

        printf("%s plays: ",
               g.current_side == SIDE_WHITE ? "White" : "Black");
        print_move(best);
        printf("\n");

        apply_move(&g, best);
    }

    printf("\nGame over. Total moves: %zu\n", g.n_moves);
    return 0;
}