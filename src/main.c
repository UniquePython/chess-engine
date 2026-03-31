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