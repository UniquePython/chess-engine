#ifndef LOC_H
#define LOC_H

typedef enum rank
{
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NO_RANK
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
    H,
    NO_FILE
} File;

typedef struct location
{
    Rank rank;
    File file;
} Loc;

static inline int index_of(Loc l)
{
    return l.rank * 8 + l.file;
}

static inline Loc loc_of(int i)
{
    return (Loc){(Rank)(i / 8), (File)(i % 8)};
}

#endif