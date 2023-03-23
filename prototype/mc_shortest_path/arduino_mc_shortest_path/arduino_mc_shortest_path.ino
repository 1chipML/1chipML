#define mc_real float

#include "monte_carlo.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void readElement(void* element, const size_t sizeOfElement) {
  unsigned char* readElement = (unsigned char*)element;
  for (uint32_t i = 0; i < sizeOfElement; ++i) {
    while (Serial.available() < 1); // Wait for element
    Serial.readBytes(&readElement[i], sizeof(unsigned char));
  }
}

void writeElement(void* element, const size_t sizeOfElement) {
  while (Serial.availableForWrite() < sizeOfElement); // wait for write
  Serial.write((unsigned char*)element, sizeOfElement);
  Serial.flush(); // wait until data is sent
}

// Test
int led = 9;         // the PWM pin the LED is attached to
int brightness = 0;  // how bright the LED is
int fadeAmount = 5;  // how many points to fade the LED by
// Test

void setup() {
  // Initialize serial
  Serial.begin(9600);

  // Test
  // pinMode(LED_BUILTIN, OUTPUT);  
  // Test
}

void loop() {
  // Test
  // digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  // delay(1000);                      // wait for a second
  // digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  // delay(1000);                      // wait for a second
  // Test


  // Receive size of board
  uint8_t nbValues;
  readElement(&nbValues, sizeof(nbValues));

  // Receive board
  int8_t boardValues[nbValues];
  for(uint8_t i = 0; i < nbValues; ++i)
  {
    readElement(&boardValues[i], sizeof(int8_t));
  }
  uint8_t nPlayers;
  readElement(&nPlayers, sizeof(nPlayers));


  Board board = {boardValues, nPlayers};
  Game game = {isValidAction, playAction, getScore, getPossibleActions, getNumPossibleActions, removeAction, getBoardSize, isDone, 0, 1};
  // Action action = mcGame(board, 1, game, 1, 1, 10); // TODO: Fix this: When number of simulations is higher than 2, doesn't work anymore
  Serial.println("Received all elements!\n");

  // uint8_t action[2] = [action.xPos, action.yPos];
  // writeElement() // TODO!
}

uint8_t totalLength = 0;
uint8_t initialBoard[9] = {
    1,  2,  3,
    1,  2,  4,
    5,  2,  1 
};

void playAction(Board* board, Action* action) {
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