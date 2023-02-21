#include "monte_carlo.h"
#include "linear_congruential_random_generator.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

mc_real calcUCB(Node* node) {
  if (node->nVisits == 0) {
    return UCB_MAX;
  }
  return node->score / node->nVisits +
         sqrt(2 * log10(node->parent->nVisits) / node->nVisits);
}

Node* findMaxUCB(Node* children, unsigned nChildren) {
  Node* maxUCBNode = children;
  for (int i = 0; i < nChildren; i++) {
    if (calcUCB(children + i) > calcUCB(maxUCBNode)) {
      maxUCBNode = children + i;
    }
  }
  return maxUCBNode;
}

Node* selectNode(Node* node) {
  Node* currNode = node;
  while (currNode->children) {
    currNode = findMaxUCB(currNode->children, currNode->nChildren);
    if (calcUCB(currNode) == UCB_MAX) {
      return currNode;
    }
  }
  return currNode;
}

void expandLeaf(Node* node, int player, Game game) {
  int nPossibleActions = game.getNumPossibleActions(node->state);
  Action possibleActions[nPossibleActions];
  game.getPossibleActions(node->state, possibleActions);

  int nValidActions = 0;
  for (int i = 0; i < nPossibleActions; i++) {
    if (!game.isValidAction(&(node->state), possibleActions + i, player)) {
      nValidActions++;
    }
  }

  // Deep copy of parent
  node->children = (struct Node*)malloc(nValidActions * sizeof(struct Node));

  nValidActions = 0;
  for (int i = 0; i < nPossibleActions; ++i) {
    if (!game.isValidAction(&(node->state), possibleActions + i, player)) {
      ((node->children) + nValidActions)->nVisits = 0;
      ((node->children) + nValidActions)->score = 0;

      // Deep copy of board
      Board board = game.playAction(node->state, possibleActions + i);
      ((node->children) + nValidActions)->state.values =
          malloc(game.getBoardSize() * sizeof(int));
      memcpy(((node->children) + nValidActions)->state.values, board.values,
             game.getBoardSize() * sizeof(int));
      ((node->children) + nValidActions)->state.nPlayers = board.nPlayers;
      ((node->children) + nValidActions)->parent = node;
      ((node->children) + nValidActions)->children = NULL;
      ((node->children) + nValidActions)->nChildren = 0;
      nValidActions++;
      node->nChildren++;
    }
  }
}

int mcEpisode(Node* node, int player, Game game) {
  int initialPlayer = player;
  // Deep copy of board
  Board simulationBoard;
  simulationBoard.values = malloc(game.getBoardSize() * sizeof(int));
  memcpy(simulationBoard.values, node->state.values,
         game.getBoardSize() * sizeof(int));
  simulationBoard.nPlayers = node->state.nPlayers;

  int nPossibleActions = game.getNumPossibleActions(simulationBoard);
  Action possibleActions[nPossibleActions];
  game.getPossibleActions(simulationBoard, possibleActions);

  // If the player is losing on a terminal node, avoid parent
  if (!(node->children) && (game.getScore(&(node->state), player) <= 0)) {
    node->parent->score = 0;
    return 0;
  }

  // Random playout
  while (nPossibleActions > 0) {
    // Pick random action
    int randomActionIdx =
        linear_congruential_random_generator() * (nPossibleActions);
    if (!game.isValidAction(&(node->state), &possibleActions[randomActionIdx],
                            player)) {
      // Play action
      // Deep copy of board
      Board board =
          game.playAction(simulationBoard, &possibleActions[randomActionIdx]);
      memcpy(simulationBoard.values, board.values,
             game.getBoardSize() * sizeof(int));
      simulationBoard.nPlayers = board.nPlayers;

      // Remove action from possible actions
      nPossibleActions -= simulationBoard.nPlayers;
      game.removeAction(randomActionIdx, possibleActions, nPossibleActions);
      int score = game.getScore(&simulationBoard, player);
      if (score > 1) { // win
        if (player == initialPlayer) {
          free(simulationBoard.values);
          return 2;
        } else {
          free(simulationBoard.values);
          return 0;
        }
      }
      player = -(player);
    }
  }
  free(simulationBoard.values);
  return 1; // draw
}

void backpropagate(Node* node, int score) {
  node->nVisits++;
  node->score += score;

  Node* currentChild = node;
  Node* currentNode = node->parent;
  while (currentNode) {
    currentNode->nVisits += 1;
    currentNode->score += score;
    currentChild = currentNode;
    currentNode = currentChild->parent;
  }
}

void freeMCTree(Node* node) {
  if (node == NULL)
    return;
  for (int i = 0; i < node->nChildren; ++i) {
    if ((node->children + i)->nChildren != 0) {
      freeMCTree(node->children + i);
    }
  }
  free(node->state.values);
  free(node->children);
}

Board mcGame(Board board, int player, Game game, int minSim, int maxSim,
              mc_real goalValue) {
  Node* node = malloc(sizeof(Node));
  node->nVisits = 0;
  node->score = 0;
  node->state = board;
  node->nChildren = 0;
  node->children = NULL;
  node->parent = NULL;
  set_linear_congruential_generator_seed(time(NULL));

  int iterations = 0;
  while (
      (node->nVisits < minSim ||
       calcUCB(findMaxUCB(node->children, node->nChildren)) < goalValue) &&
      node->nVisits < maxSim) {
    Node* selectedNode = selectNode(node);
    expandLeaf(selectedNode, player, game);
    int score = mcEpisode(selectedNode, player, game);
    backpropagate(selectedNode, score);
    iterations++;
  }

  Board retBoard;
  retBoard.values = malloc(game.getBoardSize() * sizeof(int));
  memcpy(retBoard.values,
         findMaxUCB(node->children, node->nChildren)->state.values,
         game.getBoardSize() * sizeof(int));
  freeMCTree(node);
  free(node);
  return retBoard;
}
