#include "monte_carlo.h"
#include "linear_congruential_random_generator.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef WIN
#define WIN 2
#endif
#ifndef DRAW
#define DRAW 1
#endif
#ifndef LOSS
#define LOSS 0
#endif

/**
 * This method calculates the UCB value for a node.
 * @param node The node for which the UCB is calculated.
 * @return The UCB value of the node.
 */
mc_real calcUCB(Node* node) {
  if (node->nVisits == 0) {
    return UCB_MAX;
  }
  return node->score / node->nVisits +
         sqrt(2 * log10(node->parent->nVisits) / node->nVisits);
}

/**
 * This method finds the node with the maximum UCB value among all children.
 * @param children The array of children from which the max UCB value is determined.
 * @param nChildren The number of children.
 * @return The node with the maximum UCB.
 */
Node* findMaxUCB(Node* children, unsigned nChildren) {
  Node* maxUCBNode = children;
  for (unsigned i = 0; i < nChildren; i++) {
    if (calcUCB(&children[i]) > calcUCB(maxUCBNode)) {
      maxUCBNode = &children[i];
    }
  }
  return maxUCBNode;
}

/**
 * This method selects the next node to expand from the current nodes' children.
 * @param node The current node from which the next node is selected.
 * @return The selected node.
 */
Node* selectChildren(Node* node) {
  Node* currNode = node;
  while (currNode->children) {
    currNode = findMaxUCB(currNode->children, currNode->nChildren);
    if (calcUCB(currNode) == UCB_MAX) {
      return currNode;
    }
  }
  return currNode;
}

/**
 * This method expands the leaf of the current action tree
 * @param node The current leaf to expand.
 * @param player The current player.
 * @param game The definition of the game played, the environment in which the the machine learns.
 */
void expandLeaf(Node* node, int player, Game game) {
  int nPossibleActions = game.getNumPossibleActions(node->state);
  Action possibleActions[nPossibleActions];
  game.getPossibleActions(node->state, possibleActions);

  int nValidActions = 0;
  for (unsigned i = 0; i < nPossibleActions; i++) {
    if (game.isValidAction(&(node->state), &possibleActions[i], player)) {
      nValidActions++;
    }
  }

  // Deep copy of parent
  node->children = (struct Node*)malloc(nValidActions * sizeof(struct Node));

  nValidActions = 0;
  for (unsigned i = 0; i < nPossibleActions; ++i) {
    if (game.isValidAction(&(node->state), &possibleActions[i], player)) {
      ((node->children) + nValidActions)->nVisits = 0;
      ((node->children) + nValidActions)->score = 0;

      // Deep copy of board
      Board board = game.playAction(node->state, &possibleActions[i]);
      (&node->children[nValidActions])->state.values =
          malloc(game.getBoardSize() * sizeof(int));
      Node* child = &node->children[nValidActions];
      memcpy(child->state.values, board.values,
             game.getBoardSize() * sizeof(int));
      child->state.nPlayers = board.nPlayers;
      child->parent = node;
      child->children = NULL;
      child->nChildren = 0;
      nValidActions++;
      node->nChildren++;
    }
  }
}

/**
 * This method executes a complete random playout of actions. 
 * @param node The root node from which the episode is played out.
 * @param player The current player.
 * @param game The definition of the game played, the environment in which the the machine learns.
 * @return The result of the random playout, LOSS, WIN or DRAW.
 */
int mcEpisode(Node* node, int player, Game* game) {
  int initialPlayer = player;
  // Deep copy of board
  Board simulationBoard;
  simulationBoard.values = malloc(game->getBoardSize() * sizeof(int));
  memcpy(simulationBoard.values, node->state.values,
         game->getBoardSize() * sizeof(int));
  simulationBoard.nPlayers = node->state.nPlayers;

  int nPossibleActions = game->getNumPossibleActions(simulationBoard);
  Action possibleActions[nPossibleActions];
  game->getPossibleActions(simulationBoard, possibleActions);

  // If the player is losing on a terminal node, avoid parent
  if (!node->children && (game->getScore(&node->state, player) <= 0)) {
    node->parent->score = 0;
    return 0;
  }

  // Random playout
  while (nPossibleActions > 0) {
    // Pick random action
    int randomActionIdx =
        linear_congruential_random_generator() * nPossibleActions;
    if (game->isValidAction(&node->state, &possibleActions[randomActionIdx],
                            player)) {
      // Play action
      simulationBoard =
          game->playAction(simulationBoard, &possibleActions[randomActionIdx]);

      // Remove action from possible actions
      nPossibleActions -= simulationBoard.nPlayers;
      game->removeAction(randomActionIdx, possibleActions, nPossibleActions);
      int score = game->getScore(&simulationBoard, player);
      if (score > 1) {
        free(simulationBoard.values);
        if (player == initialPlayer) {
          return WIN;
        } else {
          return LOSS;
        }
      }
      player = -(player);
    }
  }
  free(simulationBoard.values);
  return DRAW;
}

/**
 * This method backpropagates the result of the playout to the root of the action tree. 
 * @param node The leaf node from which the backpropagation begins.
 * @param score The score to backpropagate.
 */
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

/**
 * This method frees the memory used during the Monte Carlo algorithm.
 * @param node The root node from which the function starts freeing the memory.
 */
void freeMCTree(Node* node) {
  for (unsigned i = 0; i < node->nChildren; ++i) {
    if ((&node->children[i])->nChildren != 0) {
      freeMCTree(&node->children[i]);
    }
  }
  free(node->state.values);
  free(node->children);
}

/**
 * This method executes the Monte Carlo algorithm.
 * @param board The board used for the game.
 * @param player The current player.
 * @param game The definition of the game played, the environment in which the the machine learns.
 * @param minSim Minimum number of simulations to run.
 * @param maxSim Maximum number of simulations to run.
 * @return The board with the best action to play after the execution of the Monte Carlo algorithm.
 */
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
  while ((node->nVisits < minSim ||
          calcUCB(findMaxUCB(node->children, node->nChildren)) < goalValue) &&
         node->nVisits < maxSim) {
    Node* selectedNode = selectChildren(node);
    expandLeaf(selectedNode, player, game);
    int score = mcEpisode(selectedNode, player, &game);
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
