#define mc_real float

#include "monte_carlo.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void setup() {
    // Initialize serial
}

int nActions = 0;

void loop() {}

Board playAction(Board board, Action* action) {}

bool isValidAction(Board* board, Action* action, int player) {}

void getPossibleActions(Board board, Action possibleActions[]) {}

int getNumPossibleActions(Board board) {}

int getBoardSize() { return 30; }

int getScore(Board* board, int player) {
    if (nActions < 10) {return 5;} // Very good
    else if (nActions < 20) {return 3;} // Good
    else if (nActions < 30) {return 2;} // Okay
    else if (nActions < 40) {return 1;} // Not good
    return 0; // Bad
}

void removeAction(int randomActionIdx, Action* possibleActions,
                  int nPossibleActions) {}