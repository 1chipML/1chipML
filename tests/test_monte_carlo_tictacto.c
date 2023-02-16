#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/monte_carlo.h"

Board play_action(Board board, Action* action) { 
    Board childBoard;
    childBoard.values = malloc(9 * sizeof(int));
    memcpy(childBoard.values, board.values, 9 * sizeof(int));
    childBoard.values[action->x_pos * 3 + action->y_pos] = action->player;
    childBoard.nPlayers = 2;
    return childBoard;
}

char is_valid_action(Board* board, Action* action, int player) {
    if (action->player == player && board->values[action->x_pos * 3 + action->y_pos] == 0) {
        return 0;
    }
    return -1;
}

void get_possible_actions(Board board, Action possibleActions[]) {
    int nActions = 0;
    int player = 1;
    for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {
            if (board.values[x * 3 + y] == 0) {
                for (int i = 0; i < board.nPlayers; ++i) {
                    Action action;
                    action.player = player;
                    action.x_pos = x;
                    action.y_pos = y;
                    possibleActions[nActions] = action;
                    nActions++;
                    player = -player;

                }
            }
        }
    }
}

int get_num_possible_actions(Board board) {
    int nActions = 0;
    for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {
            if (board.values[x * 3 + y] != -1 && board.values[x * 3 + y] != 1) {
                nActions++;
            }
        }
    }
    return nActions * 2;
} 

/** 
 * If there is a line of X or O's 
 * 0 = loss
 * 1 = draw
 * 2 = win
*/
int get_score(Board* board, int player) { 
    // Check rows
    for (int i = 0; i < 9; i+=3) {
        if (board->values[i] == player && board->values[i + 1] == player && board->values[i + 2] == player) {
            return 2;
        }
    }
    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board->values[i] == player && board->values[i + 3] == player && board->values[i + 6] == player) {
            return 2;
        }
    }
    // Check diagonals
    if ((board->values[0] == player && board->values[4] == player && board->values[8] == player) || board->values[2] == player && board->values[4] == player && board->values[6] == player) {
        return 2;
    }
    return 1;
}

void remove_action(int randomActionIdx, Action* possibleActions, int nPossibleActions) {
    if (randomActionIdx != nPossibleActions + 1) {
        int secondAction = 0;
        if (possibleActions[randomActionIdx].player == -1) {
            secondAction = -1;
        }
        for (int i = randomActionIdx + secondAction; i < nPossibleActions; ++i) {
            possibleActions[i] = possibleActions[i + 2];
        }
    }
}

int test_play_action() {
    Board board;
    board.values = malloc(9 * sizeof(int));
    Action action;
    action.x_pos = 0;
    action.y_pos = 1;
    action.player = 1;
    board = play_action(board, &action);
    // TODO Assert if it is correct or not! 
    printf("Board: %d, %d, %d, %d, %d, %d, %d, %d, %d\n", board.values[0], board.values[1], board.values[2], board.values[3], board.values[4], board.values[5],board.values[6], board.values[7], board.values[8]);
    return 0;
}

int test_is_valid_action() {
    Board board;
    board.values = malloc(9 * sizeof(int));
    Action action;
    action.x_pos = 0;
    action.y_pos = 1;
    action.player = 1;
    char valid = is_valid_action(&board, &action, 1);
    printf("Valid?: %d\n", valid);
    if (valid == 0) {
        return 0;
    }
    return -1;
}

int test_get_possible_actions() {
    Board board;
    board.values = malloc(9 * sizeof(int));
    board.nPlayers = 2;
    int nPossibleActions = 0;

    Action* actions = malloc(18 * sizeof(Action));
    get_possible_actions(board, actions);

    // TODO assert if correct or not
    printf("Number of possible actions: %d\n", nPossibleActions);
    for (int i = 0; i < 18; ++i) {
        printf("Action: [%d,%d], player %d\n", actions->x_pos, actions->y_pos, actions->player);  
        actions++; 
    }
    
    return 0;
}

int test_get_score() {
    Board board;
    board.values = malloc(9 * sizeof(int));
    board.values[0] = 1;
    board.values[1] = 1;
    board.values[2] = 1;
    board.nPlayers = 2;
    int score = get_score(&board, 1);

    // TODO assert if correct or not
    printf("Score %d\n", score);
    free(board.values);
    return 0;
}

int main() {
    Board board;
    board.values = malloc(9 * sizeof(int));
    for (int i = 0; i < 9; ++i) {
        board.values[i] = 0;
    }
    board.nPlayers = 2;

    Game game = {
        is_valid_action, 
        play_action, 
        get_score, 
        get_possible_actions, 
        get_num_possible_actions, 
        remove_action,
    };

    board = mc_game(board, 1, game, 200, 10000, 3);
    printf("Board with max UCB: [%d, %d, %d], [%d, %d, %d], [%d, %d, %d]\n", board.values[0], board.values[1], board.values[2], board.values[3], board.values[4], board.values[5], board.values[6], board.values[7], board.values[8]);

    free(board.values);
    return 0;
}