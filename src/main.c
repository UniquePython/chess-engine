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