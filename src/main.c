#include "board.h"
#include "move.h"

int main()
{
    Board b;
    init_board(&b);
    print_board(&b);
    return 0;
}