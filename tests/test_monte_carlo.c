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

Action* get_possible_actions(Board board) {
    Action* possibleActions = malloc(4 * sizeof(Action));
    Action action1;
    action1.x_pos = 0;
    action1.player = 0;
    possibleActions[0] = action1;

    action1.x_pos = 1;
    action1.player = 0;
    possibleActions[1] = action1;

    action1.x_pos = 0;
    action1.player = 1;
    possibleActions[2] = action1;

    action1.x_pos = 1;
    action1.player = 1;
    possibleActions[3] = action1;

    return possibleActions;
}

int get_score(Board* board, int player) {
    if (board->values[0] == 0 & board->values[1] == 0) {
        return 1;
    }
    if (board->values[0] == 1) {
        return 0;
    }
    if (board->values[1] == 1) {
        return 2;
    }
}

int main() {
    Board board;
    board.values[0] = 0;
    board.values[1] = 0;
    board = monte_carlo(board, 1, is_valid_action, play_action, get_score, get_possible_actions);
    return 0;
}