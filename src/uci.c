#include "uci.h"
#include "movegen.h"
#include "search.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ENGINE_NAME "ChessEngine"
#define ENGINE_AUTHOR "Ananyo Bhattacharya"
#define SEARCH_DEPTH 3

// convert UCI square string like "e2" to Loc
static Loc parse_square(const char *s)
{
    File f = (File)(s[0] - 'a');
    Rank r = (Rank)(s[1] - '1');
    return (Loc){r, f};
}

// convert Loc to UCI square string
static void square_to_str(Loc l, char *out)
{
    out[0] = 'a' + l.file;
    out[1] = '1' + l.rank;
    out[2] = '\0';
}

// print a move in UCI format e.g. "e2e4", "e7e8q"
static void print_uci_move(Move m)
{
    char from[3], to[3];
    square_to_str(m.from, from);
    square_to_str(m.to, to);
    printf("%s%s", from, to);

    if (m.promotion != NO_PIECE)
    {
        char promo_chars[] = ".pnbrqk";
        printf("%c", promo_chars[m.promotion]);
    }
}

// match a UCI move string to a legal move in the current position
// returns true and writes to *out if found
static bool parse_move(Game *g, const char *uci_move, Move *out)
{
    Loc from = parse_square(uci_move);
    Loc to = parse_square(uci_move + 2);

    PT promo = NO_PIECE;
    if (uci_move[4] != '\0')
    {
        switch (uci_move[4])
        {
        case 'q':
            promo = QUEEN;
            break;
        case 'r':
            promo = ROOK;
            break;
        case 'b':
            promo = BISHOP;
            break;
        case 'n':
            promo = KNIGHT;
            break;
        }
    }

    // generate legal moves and find the matching one
    Move moves[256];
    int count = generate_legal_moves(g, g->current_side, moves);

    for (int i = 0; i < count; i++)
    {
        Move m = moves[i];
        if (m.from.rank == from.rank && m.from.file == from.file &&
            m.to.rank == to.rank && m.to.file == to.file &&
            m.promotion == promo)
        {
            *out = m;
            return true;
        }
    }

    return false;
}

// handle "position startpos moves e2e4 e7e5 ..."
// or     "position fen <fen>" (fen not supported yet)
static void handle_position(Game *g, const char *line)
{
    init_game(g);

    const char *moves_ptr = strstr(line, "moves");
    if (moves_ptr == NULL)
        return; // no moves, just startpos

    moves_ptr += 6; // skip "moves "

    char token[8];
    while (sscanf(moves_ptr, "%7s", token) == 1)
    {
        Move m;
        if (parse_move(g, token, &m))
            apply_move(g, m);

        // advance past this token
        moves_ptr += strlen(token);
        while (*moves_ptr == ' ')
            moves_ptr++;
    }
}

static double allocate_time_ms(int time_ms, int inc_ms)
{
    double allocated = time_ms / 30.0 + inc_ms * 0.8;
    double cap = time_ms / 3.0;
    return allocated < cap ? allocated : cap;
}

// handle "go" — run search and return bestmove
static void handle_go(Game *g, const char *line)
{
    GameStatus status = get_game_status(g);
    if (status != GAME_ONGOING)
    {
        printf("bestmove 0000\n");
        fflush(stdout);
        return;
    }

    // parse time fields
    int wtime = 0, btime = 0, winc = 0, binc = 0;
    sscanf(strstr(line, "wtime") ? strstr(line, "wtime") : "", "wtime %d", &wtime);
    sscanf(strstr(line, "btime") ? strstr(line, "btime") : "", "btime %d", &btime);
    sscanf(strstr(line, "winc") ? strstr(line, "winc") : "", "winc %d", &winc);
    sscanf(strstr(line, "binc") ? strstr(line, "binc") : "", "binc %d", &binc);

    SearchTime st;
    clock_gettime(CLOCK_MONOTONIC, &st.start);

    if (wtime > 0 || btime > 0)
    {
        // timed game
        int my_time = (g->current_side == SIDE_WHITE) ? wtime : btime;
        int my_inc = (g->current_side == SIDE_WHITE) ? winc : binc;
        st.allocated_ms = allocate_time_ms(my_time, my_inc);
    }
    else
    {
        // no time given (e.g. "go depth 5" or "go infinite") — fixed depth
        st.allocated_ms = 1e9; // effectively unlimited, rely on max_depth
    }

    fprintf(stderr, "allocated %.0fms\n", st.allocated_ms);

    Move best = search(g, 64, &st); // depth 64 = let time control it

    printf("bestmove ");
    print_uci_move(best);
    printf("\n");
    fflush(stdout);
}

void uci_loop(void)
{
    Game g;
    init_game(&g);

    char line[4096];

    while (fgets(line, sizeof(line), stdin))
    {
        // strip newline
        line[strcspn(line, "\r\n")] = '\0';

        if (strcmp(line, "uci") == 0)
        {
            printf("id name %s\n", ENGINE_NAME);
            printf("id author %s\n", ENGINE_AUTHOR);
            printf("uciok\n");
            fflush(stdout);
        }
        else if (strcmp(line, "isready") == 0)
        {
            printf("readyok\n");
            fflush(stdout);
        }
        else if (strcmp(line, "ucinewgame") == 0)
        {
            init_game(&g);
        }
        else if (strncmp(line, "position", 8) == 0)
        {
            handle_position(&g, line);
        }
        else if (strncmp(line, "go", 2) == 0)
        {
            handle_go(&g, line);
        }
        else if (strcmp(line, "quit") == 0)
        {
            break;
        }
    }
}