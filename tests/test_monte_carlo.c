#include <stdio.h>
#include <stdlib.h>
#include "../src/monte_carlo.h"

Board play_action(Board board, Action* action) { 
    Board childBoard = board;
    childBoard.values[action->x_pos] = action->player;
    return childBoard;
}

char is_valid_action(Board* board, Action* action, int player) {
    if (action->player == player & board->values[action->x_pos] == 0) {
        return 1;
    }
    return 0;
}

int main() {
    Board board;
    board.values[0] = 0;
    board.values[1] = 0;
    board = monte_carlo(board, 1, is_valid_action, play_action);
    return 0;
}