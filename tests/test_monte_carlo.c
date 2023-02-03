#include <stdio.h>
#include <stdlib.h>
#include "../src/monte_carlo.h"

int main() {
    Board board;
    board.values[0] = 0;
    board.values[1] = 0;
    board = monte_carlo(board, 1);

    printf("Board: %d, %d\n", board.values[0], board.values[1]);

    return 0;
}