#include "monte_carlo.h"
#include "linear_congruential_random_generator.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

mc_real calc_UCB(Node *node) {
    if(node->nVisits == 0) {
        return UCB_MAX; 
    }
    return node->score / node->nVisits + sqrt(2 * log10(node->parent->nVisits) / node->nVisits); 
}

Node* find_max_UCB(Node *children, unsigned nChildren) {
    Node *max_ucb_node = children;
    for (int i = 0; i < nChildren; i++) {
        if (calc_UCB(children + i) > calc_UCB(max_ucb_node)) {
            max_ucb_node = children + i;
        }
    }
    return max_ucb_node;
}

Node* select_node(Node *node) {
    Node *curr_node = node;
    while (curr_node->children) { 
        curr_node = find_max_UCB(curr_node->children, curr_node->nChildren);
        if (calc_UCB(curr_node) == UCB_MAX) {
            return curr_node;
        }
    }
    return curr_node;
}

void expand_leaf(Node* node, int player, Game game) {
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
    node->children = (struct Node*) malloc(nValidActions * sizeof(struct Node));
    
    nValidActions  = 0;
    for (int i = 0; i < nPossibleActions; ++i) {
        if (!game.isValidAction(&(node->state), possibleActions + i, player)) {
            ((node->children) + nValidActions)->nVisits = 0;
            ((node->children) + nValidActions)->score = 0;

            // Deep copy of board
            Board board = game.playAction(node->state, possibleActions + i);
            ((node->children) + nValidActions)->state.values = malloc(game.getBoardSize() * sizeof(int));
            memcpy(((node->children) + nValidActions)->state.values, board.values, game.getBoardSize() * sizeof(int));
            ((node->children) + nValidActions)->state.nPlayers = board.nPlayers;
            ((node->children) + nValidActions)->parent = node;
            ((node->children) + nValidActions)->children = NULL;
            ((node->children) + nValidActions)->nChildren = 0;
            nValidActions++;
            node->nChildren++;
        }
    }
}

int mc_episode(Node* node, int player, Game game) {
    int initialPlayer = player;
    // Deep copy of board
    Board simulationBoard;
    simulationBoard.values = malloc(game.getBoardSize() * sizeof(int));
    memcpy(simulationBoard.values, node->state.values, game.getBoardSize() * sizeof(int));
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
        int random_action_idx = linear_congruential_random_generator() * (nPossibleActions);
        if (!game.isValidAction(&(node->state), &possibleActions[random_action_idx], player)) {
            // Play action 
            // Deep copy of board 
            Board board = game.playAction(simulationBoard, &possibleActions[random_action_idx]);
            memcpy(simulationBoard.values, board.values, game.getBoardSize() * sizeof(int));
            simulationBoard.nPlayers = board.nPlayers;

            // Remove action from possible actions
            nPossibleActions -= simulationBoard.nPlayers;
            game.removeAction(random_action_idx, possibleActions, nPossibleActions);
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

void backpropagate(Node *node, int score) {
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

void free_mc_tree(Node* node)
{
    if (node == NULL) return;
    for (int i = 0; i < node->nChildren; ++i) {
        if ((node->children + i)->nChildren != 0) {
            free_mc_tree(node->children + i);
        } 
    }
    free(node->state.values);
    free(node->children);
}

Board mc_game(Board board, int player, Game game, int minSim, int maxSim, mc_real goalValue) {
    Node *node = malloc(sizeof(Node));
    node->nVisits = 0;
    node->score = 0;
    node->state = board;
    node->nChildren = 0;
    node->children = NULL;
    node->parent = NULL;
    set_linear_congruential_generator_seed(time(NULL));

    int iterations = 0;
    while ((node->nVisits < minSim || calc_UCB(find_max_UCB(node->children, node->nChildren)) < goalValue) && node->nVisits < maxSim) {
        Node* selected_node = select_node(node);
        expand_leaf(selected_node, player, game);
        int score = mc_episode(selected_node, player, game);
        backpropagate(selected_node, score);
        iterations++;
    }

    Board retBoard;
    retBoard.values = malloc(game.getBoardSize() * sizeof(int));
    memcpy(retBoard.values, find_max_UCB(node->children, node->nChildren)->state.values, game.getBoardSize() * sizeof(int));
    free_mc_tree(node);
    free(node);
    return retBoard;
}