#define mc_real float

#include "monte_carlo.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void setup() {
  // Initialize serial
}

void loop() {}

uint8_t totalLength = 0;
uint8_t initialBoard[9] = {
    1,  2,  3,
    1,  2,  4,
    5,  2,  1 
};

Board playAction(Board* board, Action* action) {
  totalLength += board->values[action->xPos * 3 + action->yPos];
  for (uint8_t i = 0; i < 3; ++i) {
    for (uint8_t j = 0; j < 3; ++j) {
      if (board->values[i * 3 + j] == -1) {
        board->values[i * 3 + j] = initialBoard[i * 3 + j];
      }
    }
  }
  board->values[action->xPos * 3 + action->yPos] = -1;
}

static bool isValidAction(Board* board, Action* action, int player) {
  uint8_t currentPos = 0;
  for (uint8_t i = 0; i < 3; ++i) {
    for (uint8_t j = 0; j < 3; ++j) {
      if (board->values[i * 3 + j] == -1) {
        currentPos = i * 3 + j;
        break;
      }
    }
  }
  uint8_t actionPos = action->xPos * 3 + action->yPos;
  if (board->values[actionPos] != -1) {
    // Check if action is above or below current position
    if ((currentPos < 3 && currentPos + 3 == actionPos) ||
        (currentPos < 6 &&
         (currentPos + 3 == actionPos || currentPos - 1 == actionPos)) ||
        (currentPos >= 6 && currentPos - 3 == actionPos)) {
      return 1;
    }
    // Check if action is right or left from current position
    else if ((currentPos == 0 || currentPos == 3 || currentPos == 6) &&
             (currentPos + 1 == actionPos)) {
      return 1;
    } else if ((currentPos == 1 || currentPos == 4 || currentPos == 7) &&
               (currentPos + 1 == actionPos || currentPos - 1 == actionPos)) {
      return 1;
    } else if ((currentPos == 2 || currentPos == 5 || currentPos == 8) &&
               (currentPos - 1 == actionPos)) {
      return 1;
    }
  }
  return 0;
}

void getPossibleActions(Board board, Action possibleActions[]) {
  int nActions = 0;
  for (int x = 0; x < 3; ++x) {
    for (int y = 0; y < 3; ++y) {
      if (board.values[x * 3 + y] != -1) {
        possibleActions[nActions].player = 0;
        possibleActions[nActions].xPos = x;
        possibleActions[nActions].yPos = y;
        nActions++;
      }
    }
  } 
}

static int getNumPossibleActions(Board board) {
  return 8;
}

int getBoardSize() { return 9; }

static bool isDone(Board* board) {
  int8_t value = board->values[8];
  if (board->values[8] == -1) return true;
  return false;
}

int getScore(Board* board, int player) {
  if (board->values[8] == -1) {
    if (totalLength == 6)
      return 4; // Best path
    if (totalLength >= 9)
      return 2; // Worst path
    if (totalLength >= 7)
      return 3; // Okay path
  }
  return 0;
}

void removeAction(int randomActionIdx, Action* possibleActions,
                  int nPossibleActions) {
  int nActions = 0;
  for (int x = 0; x < 3; ++x) {
    for (int y = 0; y < 3; ++y) {
      if (x * 3 + y != randomActionIdx) {
        possibleActions[nActions].player = 0;
        possibleActions[nActions].xPos = x;
        possibleActions[nActions].yPos = y;
        nActions++;
      }
    }
  } 
}