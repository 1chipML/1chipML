#include "arduino_serial_port.hpp"
#include "monte_carlo.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_LENGTH 3
#define BOARD_SIZE (BOARD_LENGTH * BOARD_LENGTH) // 3 x 3 board

uint8_t totalLength = 0;
uint8_t initialBoard[BOARD_SIZE];

void setup() {
  // Initialize serial
  Serial.begin(9600);
}

void loop() {
  // Receive size of board
  uint8_t nbValues;
  readElement(&nbValues, sizeof(nbValues));

  // Receive board
  int8_t boardValues[nbValues];
  readArray(nbValues, &boardValues, sizeof(int8_t));
  uint8_t nPlayers;
  readElement(&nPlayers, sizeof(nPlayers));

  initialBoard[0] = 1;
  for (int i = 1; i < nbValues; ++i) {
    initialBoard[i] = boardValues[i];
  }

  int minSimulation = 10;
  int maxSimulation = 48;
  int targetScore = 10;

  Board board = {boardValues, nPlayers};
  Game game = {isValidAction,
               playAction,
               getScore,
               getPossibleActions,
               getNumPossibleActions,
               removeAction,
               getBoardSize,
               isDone,
               0,
               1};
  Action action =
      mcGame(board, 0, game, minSimulation, maxSimulation, targetScore);

  // Return best action
  writeElement(&action.xPos, sizeof(uint8_t));
  writeElement(&action.yPos, sizeof(uint8_t));
}

uint8_t findCurrentPosition(const Board* board) {
  for (uint8_t i = 0; i < BOARD_SIZE; ++i) {
    if (board->values[i] == -1) {
      return i;
    }
  }
  return 0;
}

// Current position is -1, if a node has been visited, it is -2
void playAction(Board* board, Action* action) {
  totalLength += initialBoard[action->xPos * BOARD_LENGTH + action->yPos];
  uint8_t currentPos = findCurrentPosition(board);
  board->values[currentPos] = -2;
  board->values[action->xPos * BOARD_LENGTH + action->yPos] = -1;
}

bool isValidAction(Board* board, Action* action, int player) {
  uint8_t currentPos = findCurrentPosition(board);
  uint8_t actionPos = action->xPos * BOARD_LENGTH + action->yPos;
  if (actionPos != currentPos && board->values[actionPos] != -2) {
    // Check if action is above or below current position
    if ((currentPos < 3 && currentPos + 3 == actionPos) ||
        ((currentPos < 6 && currentPos >= 3) &&
         (currentPos + 3 == actionPos || currentPos - 3 == actionPos)) ||
        (currentPos >= 6 && currentPos - 3 == actionPos)) {
      return true;
    }
    // Check if action is right or left from current position
    else if ((currentPos == 0 || currentPos == 3 || currentPos == 6) &&
             (currentPos + 1 == actionPos)) {
      return true;
    } else if ((currentPos == 1 || currentPos == 4 || currentPos == 7) &&
               (currentPos + 1 == actionPos || currentPos - 1 == actionPos)) {
      return true;
    } else if ((currentPos == 2 || currentPos == 5 || currentPos == 8) &&
               (currentPos - 1 == actionPos)) {
      return true;
    }
  }
  return false;
}

void getPossibleActions(Board* board, Action* possibleActions) {
  int nActions = 0;
  for (int x = 0; x < BOARD_LENGTH; ++x) {
    for (int y = 0; y < BOARD_LENGTH; ++y) {
      if (board->values[x * BOARD_LENGTH + y] >= 0) {
        possibleActions[nActions].player = 0;
        possibleActions[nActions].xPos = x;
        possibleActions[nActions].yPos = y;
        nActions++;
      }
    }
  }
}

bool isDone(Board* board) {
  return board->values[BOARD_SIZE - 1] < 0;
}

int getNumPossibleActions(Board* board) {
  if (isDone(board)) {
    return 0;
  }
  int nActions = 0;
  for (int i = 0; i < BOARD_SIZE; ++i) {
    if (board->values[i] >= 0) {
      nActions++;
    }
  }
  return nActions;
}

int getBoardSize() { return BOARD_SIZE; }

bool isStuck(Board* board) {
  uint8_t currentPos = findCurrentPosition(board);
  switch(currentPos)
  {
    case 0:
      return (board->values[currentPos + 1] == -2 &&
      board->values[currentPos + 3] == -2);
    case 1:
      return (board->values[currentPos - 1] == -2 &&
             board->values[currentPos + 1] == -2 &&
             board->values[currentPos + 3] == -2);
    case 2:
      return (currentPos == 2 && board->values[currentPos - 1] == -2 &&
             board->values[currentPos + 3] == -2);
    case 3:
      return (board->values[currentPos - 3] == -2 &&
             board->values[currentPos + 1] == -2 &&
             board->values[currentPos + 3] == -2);
    case 4:
      return (board->values[currentPos - 3] == -2 &&
             board->values[currentPos - 1] == -2 &&
             board->values[currentPos + 1] == -2 &&
             board->values[currentPos + 3] == -2);
    case 5:
      return (board->values[currentPos - 3] == -2 &&
             board->values[currentPos - 1] == -2 &&
             board->values[currentPos + 3] == -2);
    case 6:
      return (board->values[currentPos - 3] == -2 &&
             board->values[currentPos + 1] == -2);
    case 7:
      return (board->values[currentPos - 3] == -2 &&
             board->values[currentPos - 1] == -2 &&
             board->values[currentPos + 1] == -2);
    default:
      return false;

  }
}

int getScore(Board* board, int player) {
  int score = 0;
  if (isStuck(board)) {
    totalLength = 0;
    score = 2;
  } else if (isDone(board)) {
    if (totalLength == 6) {
      score = 10; // Best path
    }
    if (totalLength >= 9) {
      score = 3; // Worst path
    }
    if (totalLength >= 7) {
      score = 4; // Okay path
    }
    totalLength = 0;
  }
  return score;
}

void removeAction(int randomActionIdx, Action* possibleActions,
                  int nPossibleActions) {
  if (randomActionIdx != nPossibleActions + 1) {
    for (int i = randomActionIdx; i < nPossibleActions; ++i) {
      possibleActions[i] = possibleActions[i + 1];
    }
  }
}