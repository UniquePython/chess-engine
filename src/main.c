// --- Individual Pieces ------------>

typedef enum piece_colour
{
    NO_COLOR,
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

// --- Board ------------>

typedef enum row
{
    ONE = 1,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT
} Row;

typedef enum column
{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H
} Col;

typedef struct location
{
    Row row;
    Col col;
} Loc;

typedef struct square
{
    Loc location;
    Piece piece;
} Square;

Square board[64];