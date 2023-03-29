#include "arduino_serial_port.h"
#include "monte_carlo.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_LENGTH 3
#define BOARD_SIZE 9 // 3 x 3 board


uint8_t totalLength = 0;
uint8_t initialBoard[9];

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
      mcGame(board, 1, game, minSimulation, maxSimulation, targetScore);

  // Return best action
  writeElement(&action.xPos, sizeof(uint8_t));
  writeElement(&action.yPos, sizeof(uint8_t));
}

uint8_t findCurrentPosition(Board* board) {
  for (uint8_t i = 0; i < BOARD_SIZE - 1; ++i) {
    if (board->values[i] == -1) {
      return i;
    }
  }
  return 0;
}

void playAction(Board* board, Action* action) {
  totalLength += board->values[action->xPos * BOARD_LENGTH + action->yPos];
  uint8_t currentPos = findCurrentPosition(board);
  board->values[currentPos] = initialBoard[currentPos];
  board->values[action->xPos * BOARD_LENGTH + action->yPos] = -1;
}

bool isValidAction(Board* board, Action* action, int player) {
  uint8_t currentPos = findCurrentPosition(board);
  uint8_t actionPos = action->xPos * BOARD_LENGTH + action->yPos;
  if (board->values[actionPos] != -1) {
    // Check if action is above or below current position
    if ((currentPos < 3 && currentPos + 3 == actionPos) ||
        ((currentPos < 6 && currentPos >= 3) &&
         (currentPos + 3 == actionPos || currentPos - 1 == actionPos)) ||
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
      if (board->values[x * BOARD_LENGTH + y] != -1) {
        possibleActions[nActions].player = 0;
        possibleActions[nActions].xPos = x;
        possibleActions[nActions].yPos = y;
        nActions++;
      }
    }
  }
}

int getNumPossibleActions() { return BOARD_SIZE - 1; }

int getBoardSize() { return BOARD_SIZE; }

bool isDone(Board* board) {
  if (board->values[BOARD_SIZE - 1] == -1)
    return true;
  return false;
}

int getScore(Board* board, int player) {
  if (isDone(board)) {
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
  for (int x = 0; x < BOARD_LENGTH; ++x) {
    for (int y = 0; y < BOARD_LENGTH; ++y) {
      if (x * BOARD_LENGTH + y != randomActionIdx) {
        possibleActions[nActions].player = 0;
        possibleActions[nActions].xPos = x;
        possibleActions[nActions].yPos = y;
        nActions++;
      }
    }
  }
}