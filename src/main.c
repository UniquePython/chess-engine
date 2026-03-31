#include <stdbool.h>
#include <stdio.h>

// --- Individual Pieces ------------>

typedef enum piece_colour
{
    NO_COLOUR,
    WHITE,
    BLACK
} PC;

typedef enum piece_type
{
    NO_PIECE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} PT;

typedef struct piece
{
    PC colour;
    PT type;
} Piece;

// --- Piece Creation Helpers ------------>

static inline Piece make_piece(PC colour, PT type)
{
    return (Piece){colour, type};
}

static inline Piece no_piece(void)
{
    return make_piece(NO_COLOUR, NO_PIECE);
}

static inline Piece white(PT type)
{
    return make_piece(WHITE, type);
}

static inline Piece black(PT type)
{
    return make_piece(BLACK, type);
}

// --- Piece Comparison Helpers ------------>

static inline bool is_empty(Piece p)
{
    return p.type == NO_PIECE && p.colour == NO_COLOUR;
}

static inline bool is_white(Piece p)
{
    return p.colour == WHITE;
}

static inline bool is_black(Piece p)
{
    return p.colour == BLACK;
}

static inline bool is_same_color(Piece a, Piece b)
{
    return a.colour != NO_COLOUR && a.colour == b.colour;
}

static inline bool is_opposite_color(Piece a, Piece b)
{
    return a.colour != NO_COLOUR && b.colour != NO_COLOUR && a.colour != b.colour;
}

// --- Piece Info Helpers ------------>

static inline PT type_of(Piece p)
{
    return p.type;
}

static inline PC colour_of(Piece p)
{
    return p.colour;
}

// --- Board ------------>

typedef enum rank
{
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT
} Rank;

typedef enum file
{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H
} File;

typedef struct location
{
    Rank rank;
    File file;
} Loc;

typedef struct
{
    Piece squares[64];
} Board;

// --- Board Helpers ------------>

static inline int index_of(Loc s)
{
    return s.rank * 8 + s.file;
}

static inline Loc loc_of(int i)
{
    return (Loc){i / 8, i % 8};
}

// --- Getter / Setter ------------>

static inline Piece get(Board *b, Loc l)
{
    return b->squares[index_of(l)];
}

static inline void set(Board *b, Loc l, Piece p)
{
    b->squares[index_of(l)] = p;
}

// --- Move ------------>

typedef struct
{
    Loc from;
    Loc to;
    PT promotion;
    bool is_castle;
    bool is_en_passant;
} Move;

// --- Core logic ------------>

static inline void init_board(Board *b)
{
    for (int i = 0; i < 64; i++)
        b->squares[i] = no_piece();

    // Pawns
    for (File f = A; f <= H; f++)
    {
        set(b, (Loc){TWO, f}, white(PAWN));
        set(b, (Loc){SEVEN, f}, black(PAWN));
    }

    // White back rank
    set(b, (Loc){ONE, A}, white(ROOK));
    set(b, (Loc){ONE, B}, white(KNIGHT));
    set(b, (Loc){ONE, C}, white(BISHOP));
    set(b, (Loc){ONE, D}, white(QUEEN));
    set(b, (Loc){ONE, E}, white(KING));
    set(b, (Loc){ONE, F}, white(BISHOP));
    set(b, (Loc){ONE, G}, white(KNIGHT));
    set(b, (Loc){ONE, H}, white(ROOK));

    // Black back rank
    set(b, (Loc){EIGHT, A}, black(ROOK));
    set(b, (Loc){EIGHT, B}, black(KNIGHT));
    set(b, (Loc){EIGHT, C}, black(BISHOP));
    set(b, (Loc){EIGHT, D}, black(QUEEN));
    set(b, (Loc){EIGHT, E}, black(KING));
    set(b, (Loc){EIGHT, F}, black(BISHOP));
    set(b, (Loc){EIGHT, G}, black(KNIGHT));
    set(b, (Loc){EIGHT, H}, black(ROOK));
}

// --- Debug ------------>

static inline char piece_to_char(Piece p)
{
    if (is_empty(p))
        return '.';

    char c;

    switch (p.type)
    {
    case PAWN:
        c = 'P';
        break;
    case KNIGHT:
        c = 'N';
        break;
    case BISHOP:
        c = 'B';
        break;
    case ROOK:
        c = 'R';
        break;
    case QUEEN:
        c = 'Q';
        break;
    case KING:
        c = 'K';
        break;
    default:
        c = '?';
        break;
    }

    if (p.colour == BLACK)
        c = c + ('a' - 'A'); // lowercase for black

    return c;
}

static inline void print_board(Board *b)
{
    for (int r = EIGHT; r >= ONE; r--)
    {
        printf("%d  ", r + 1);

        for (int f = A; f <= H; f++)
        {
            Piece p = get(b, (Loc){r, f});
            printf("%c ", piece_to_char(p));
        }

        printf("\n");
    }

    printf("\n   a b c d e f g h\n");
}

// --- Entry point ------------>

int main(void)
{
    Board b;
    init_board(&b);
    print_board(&b);

    return 0;
}