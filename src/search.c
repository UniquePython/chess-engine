#include "search.h"
#include "movegen.h"
#include "eval.h"

#include <stdio.h>

#define INFINITY_SCORE 1000000
#define CHECKMATE_SCORE 999000

static const int VICTIM_SCORE[7] = {0, 100, 200, 300, 400, 500, 600};
static const int ATTACKER_SCORE[7] = {0, 6, 5, 4, 3, 2, 1};

static int move_score(Game *g, Move m)
{
    if (m.is_en_passant)
        return VICTIM_SCORE[PAWN] + ATTACKER_SCORE[PAWN];

    Piece victim = get(&g->board, m.to);
    if (!is_empty(victim))
    {
        Piece attacker = get(&g->board, m.from);
        return VICTIM_SCORE[victim.type] + ATTACKER_SCORE[attacker.type];
    }

    return 0; // quiet move — sorted to the back
}

static void sort_moves(Game *g, Move *moves, int count)
{
    for (int i = 1; i < count; i++)
    {
        Move key = moves[i];
        int key_score = move_score(g, key);
        int j = i - 1;
        while (j >= 0 && move_score(g, moves[j]) < key_score)
        {
            moves[j + 1] = moves[j];
            j--;
        }
        moves[j + 1] = key;
    }
}

static int quiescence(Game *g, int alpha, int beta, bool maximizing)
{
    int stand_pat = evaluate(g);

    if (maximizing)
    {
        if (stand_pat >= beta)
            return beta;
        if (stand_pat > alpha)
            alpha = stand_pat;
    }
    else
    {
        if (stand_pat <= alpha)
            return alpha;
        if (stand_pat < beta)
            beta = stand_pat;
    }

    // Generate only captures
    Move moves[256];
    int count = generate_legal_moves(g, g->current_side, moves);
    sort_moves(g, moves, count);

    for (int i = 0; i < count; i++)
    {
        // Skip non-captures
        Piece target = get(&g->board, moves[i].to);
        if (is_empty(target) && !moves[i].is_en_passant)
            continue;

        apply_move(g, moves[i]);
        int score = quiescence(g, alpha, beta, !maximizing);
        undo_move(g);

        if (maximizing)
        {
            if (score > alpha)
                alpha = score;
            if (alpha >= beta)
                return beta;
        }
        else
        {
            if (score < beta)
                beta = score;
            if (beta <= alpha)
                return alpha;
        }
    }

    return maximizing ? alpha : beta;
}

static int node_count = 0;
static SearchTime *g_st = NULL;
static bool search_aborted = false;

static int minimax(Game *g, int depth, int alpha, int beta, bool maximizing)
{
    // check time every 1024 nodes
    if ((++node_count & 1023) == 0 && g_st && time_up(g_st))
    {
        search_aborted = true;
        return 0; // score doesn't matter, discard this result
    }

    if (search_aborted)
        return 0;

    if (depth == 0)
        return quiescence(g, alpha, beta, maximizing);

    Move moves[1024];
    int count = generate_legal_moves(g, g->current_side, moves);
    sort_moves(g, moves, count);

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

Move search(Game *g, int max_depth, SearchTime *st)
{
    bool maximizing = (g->current_side == SIDE_WHITE);
    Move best_move = {0};

    g_st = st;
    search_aborted = false;
    node_count = 0;

    for (int depth = 1; depth <= max_depth; depth++)
    {
        Move current_best = {0};
        int best_score = maximizing ? -INFINITY_SCORE : INFINITY_SCORE;

        Move moves[1024];
        int count = generate_legal_moves(g, g->current_side, moves);
        sort_moves(g, moves, count);

        // put best move from previous iteration first
        if (depth > 1)
        {
            for (int i = 0; i < count; i++)
            {
                if (moves[i].from.rank == best_move.from.rank &&
                    moves[i].from.file == best_move.from.file &&
                    moves[i].to.rank == best_move.to.rank &&
                    moves[i].to.file == best_move.to.file &&
                    moves[i].promotion == best_move.promotion)
                {
                    Move tmp = moves[0];
                    moves[0] = moves[i];
                    moves[i] = tmp;
                    break;
                }
            }
        }

        for (int i = 0; i < count; i++)
        {
            apply_move(g, moves[i]);
            int score = minimax(g, depth - 1, -INFINITY_SCORE, INFINITY_SCORE, !maximizing);
            undo_move(g);

            if (search_aborted)
                goto done; // discard incomplete depth, keep last best_move

            if (maximizing ? score > best_score : score < best_score)
            {
                best_score = score;
                current_best = moves[i];
            }
        }

        best_move = current_best; // only update on fully completed depth
        fprintf(stderr, "depth %d: score %d  (%.1fms)\n", depth, best_score, elapsed_ms(st));

        if (time_up(st))
            break;
    }

done:
    return best_move;
}
