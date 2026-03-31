#include <stdbool.h>

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
    return p.type == NO_PIECE;
}

static inline bool is_white(Piece p)
{
    return p.colour == WHITE;
}

static inline bool is_black(Piece p)
{
    return p.colour == BLACK;
}

static inline bool same_color(Piece a, Piece b)
{
    return a.colour == b.colour;
}

// --- Board ------------>

typedef struct
{
    Piece squares[64];
} Board;