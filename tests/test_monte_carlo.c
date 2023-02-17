#include "../src/monte_carlo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Board play_action(Board board, Action* action) {
  Board childBoard;
  childBoard.values = malloc(9 * sizeof(int));
  memcpy(childBoard.values, board.values, 9 * sizeof(int));
  childBoard.values[action->x_pos * 3 + action->y_pos] = action->player;
  childBoard.nPlayers = 2;
  return childBoard;
}

char is_valid_action(Board* board, Action* action, int player) {
  if (action->player == player &&
      board->values[action->x_pos * 3 + action->y_pos] == 0) {
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

int get_board_size() { return 9; }
/**
 * If there is a line of X or O's
 * 0 = loss
 * 1 = draw
 * 2 = win
 */
int get_score(Board* board, int player) {
  // Check rows
  for (int i = 0; i < 9; i += 3) {
    if (board->values[i] == player && board->values[i + 1] == player &&
        board->values[i + 2] == player) {
      return 2;
    }
  }
  // Check columns
  for (int i = 0; i < 3; i++) {
    if (board->values[i] == player && board->values[i + 3] == player &&
        board->values[i + 6] == player) {
      return 2;
    }
  }
  // Check diagonals
  if ((board->values[0] == player && board->values[4] == player &&
       board->values[8] == player) ||
      board->values[2] == player && board->values[4] == player &&
          board->values[6] == player) {
    return 2;
  }
  return 1;
}

void remove_action(int randomActionIdx, Action* possibleActions,
                   int nPossibleActions) {
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

int test_mc(int minSimulation, int maxSimulation, int targetScore) {
  Board board;
  board.values = malloc(9 * sizeof(int));
  board.values[0] = 1;
  board.values[1] = 1;
  for (int i = 2; i < 9; ++i) {
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
      get_board_size,
  };

  board = mc_game(board, 1, game, minSimulation, maxSimulation, targetScore);
  if (board.values[2] == 1) {
    free(board.values);
    printf("Success : %s()\n", __func__);
    return 0;
  } else {
    printf("Fail : %s(), expected best action to be: [%d, %d, %d] [%d, %d, %d] "
           "[%d, %d, %d]\n",
           __func__, 1, 1, 1, 0, 0, 0, 0, 0, 0);
    return 1;
  }
}

int main() { test_mc(9, 50, 4); }
