#include <monte_carlo.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void playAction(Board* board, Action* action) {
  board->values[action->xPos * 3 + action->yPos] = action->player;
}

static bool isValidAction(Board* board, Action* action, int player) {
  if (action->player == player &&
      board->values[action->xPos * 3 + action->yPos] == 0) {
    return 1;
  }
  return 0;
}

void getPossibleActions(Board board, Action possibleActions[]) {
  int nActions = 0;
  int player = 1;
  for (int x = 0; x < 3; ++x) {
    for (int y = 0; y < 3; ++y) {
      if (board.values[x * 3 + y] == 0) {
        for (int i = 0; i < board.nPlayers; ++i) {
          possibleActions[nActions].player = player;
          possibleActions[nActions].xPos = x;
          possibleActions[nActions].yPos = y;
          nActions++;
          player = -player;
        }
      }
    }
  }
}

static int getNumPossibleActions(Board board) {
  int nActions = 0;
  for (int x = 0; x < 3; ++x) {
    for (int y = 0; y < 3; ++y) {
      if (board.values[x * 3 + y] == 0) {
        nActions++;
      }
    }
  }
  return nActions * board.nPlayers;
}

static int getBoardSize() { return 9; }

static bool isDone(Board* board) {
  for (int i = 0; i < 9; ++i) {
    if (board->values[i] == 0) {
      return false;
    }
  }
  return true;
}

/**
 * If there is a line of X or O's
 * 1 = draw
 * 2 = win
 * 3 = win with only 3 plays
 */
int getScore(Board* board, int player) {
  // Check rows
  for (int i = 0; i < 7; i += 3) {
    if (board->values[i] == player && board->values[i + 1] == player &&
        board->values[i + 2] == player) {
      int nPlays = 0;
      for (int j = 0; j < 9; ++j) {
        if (board->values[j] == player) {
          ++nPlays;
        }
      }
      return (nPlays == 3) ? 3 : 2;
    }
  }
  // Check columns
  for (int i = 0; i < 3; i++) {
    if (board->values[i] == player && board->values[i + 3] == player &&
        board->values[i + 6] == player) {
      int nPlays = 0;
      for (int j = 0; j < 9; ++j) {
        if (board->values[j] == player) {
          ++nPlays;
        }
      }
      return (nPlays == 3) ? 3 : 2;
    }
  }
  // Check diagonals
  if ((board->values[0] == player && board->values[4] == player &&
       board->values[8] == player) ||
      (board->values[2] == player && board->values[4] == player &&
          board->values[6] == player)) {
    int nPlays = 0;
    for (int j = 0; j < 9; ++j) {
      if (board->values[j] == player) {
        ++nPlays;
      }
    }
    return (nPlays == 3) ? 3 : 2;
  }
  return 1;
}

void removeAction(int randomActionIdx, Action* possibleActions,
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

int testMC(int minSimulation, int maxSimulation, int targetScore) {
  Board board;
  board.values = calloc(9, sizeof(int));
  board.values[0] = 1;
  board.values[1] = 1;
  board.nPlayers = 2;

  Game game = {
      isValidAction,         playAction,   getScore,     getPossibleActions,
      getNumPossibleActions, removeAction, getBoardSize, isDone, 0, 1
  };

  Action action =
      mcGame(board, 1, game, minSimulation, maxSimulation, targetScore);
  free(board.values);
  if (action.xPos == 0 && action.yPos == 2) {
    printf("Success : %s()\n", __func__);
    return 0;
  } else {
    printf("Fail : %s(), expected best action to be: Player: %d, [%d, %d]\n",
           __func__, 1, 0, 2);
    return 1;
  }
}

int main() {
  const int minSimul = 10;
  const int maxSimul = 500;
  const mc_real targetScore = 5;
  testMC(minSimul, maxSimul, targetScore);

  // Board board;
  // board.values = calloc(9, sizeof(int));

  // Game game = {
  //     isValidAction,         playAction,   getScore,     getPossibleActions,
  //     getNumPossibleActions, removeAction, getBoardSize, isDone,
  // };

  // Action action =
  //     mcGame(board, 1, game, minSimul, maxSimul, targetScore);

  // printf("Best action with %d simulations: Player: %d, [%d, %d]\n", maxSimul,
  // action.player, action.xPos, action.yPos);
}
