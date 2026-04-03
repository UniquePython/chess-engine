#include "search.h"
#include "movegen.h"
#include "eval.h"

#include <stdio.h>

#define INFINITY_SCORE 1000000
#define CHECKMATE_SCORE 999000

static int minimax(Game *g, int depth, int alpha, int beta, bool maximizing)
{
    if (depth == 0)
        return evaluate(g);

    Move moves[256];
    int count = generate_legal_moves(g, g->current_side, moves);

    if (count == 0)
    {
        if (is_in_check(g, g->current_side))
            // checkmate - worse than any material loss, offset by depth
            // so engine prefers faster checkmates
            return maximizing ? -(CHECKMATE_SCORE + depth) : (CHECKMATE_SCORE + depth);
        else
            return 0; // stalemate
    }

    if (maximizing)
    {
        int best = -INFINITY_SCORE;

        for (int i = 0; i < count; i++)
        {
            apply_move(g, moves[i]);
            int score = minimax(g, depth - 1, alpha, beta, false);
            undo_move(g);

            if (score > best)
                best = score;
            if (score > alpha)
                alpha = score;
            if (beta <= alpha)
                break; // beta cutoff
        }

        return best;
    }
    else
    {
        int best = INFINITY_SCORE;

        for (int i = 0; i < count; i++)
        {
            apply_move(g, moves[i]);
            int score = minimax(g, depth - 1, alpha, beta, true);
            undo_move(g);

            if (score < best)
                best = score;
            if (score < beta)
                beta = score;
            if (beta <= alpha)
                break; // alpha cutoff
        }

        return best;
    }
}

Move search(Game *g, int depth)
{
    bool maximizing = (g->current_side == SIDE_WHITE);
    Move best_move = {0};
    int best_score = maximizing ? -INFINITY_SCORE : INFINITY_SCORE;

    Move moves[256];
    int count = generate_legal_moves(g, g->current_side, moves);

    for (int i = 0; i < count; i++)
    {
        apply_move(g, moves[i]);
        int score = minimax(g, depth - 1, -INFINITY_SCORE, INFINITY_SCORE, g->current_side == SIDE_WHITE);
        undo_move(g);

        if (maximizing ? score > best_score : score < best_score)
        {
            best_score = score;
            best_move = moves[i];
        }
    }

    // fprintf(stderr, "best score: %d\n", best_score);
    return best_move;
}