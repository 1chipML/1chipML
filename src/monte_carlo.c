#include "monte_carlo.h"
#include "linear_congruential_random_generator.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int calc_UCB(Node* node) {
    if(node->nVisits == 0) {
        return __INT_MAX__; 
    }
    return node->score / node->nVisits;
    // return node->score / node->nVisits + sqrt(2 * log10(node->parent->nVisits) / node->nVisits);
}

Node* find_max_UCB(Node *children){
    Node *max_ucb_node = children;

    for (int i = 0; i < sizeof(children); i++) {
        if (calc_UCB(children + i) > calc_UCB(max_ucb_node)) {
            max_ucb_node = children + i;
        }
    }
    return max_ucb_node;
}

Node* select_node(Node *node) {
    Node *curr_node = node;
    while (curr_node->children) { 
        curr_node = find_max_UCB(curr_node->children);
        if (calc_UCB(curr_node) == __INT_MAX__) {
            return curr_node;
        }
    }
    return curr_node;
}

void expand_leaf(Node* node, int player, char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board, Action*), void (*getPossibleActions)(Board, Action*), int (*getNumPossibleActions)(Board)) {
    int nPossibleActions = getNumPossibleActions(node->state);
    Action possibleActions[nPossibleActions];
    getPossibleActions(node->state, possibleActions);

    int nValidActions = 0;
    for (int i = 0; i < nPossibleActions; i++) {
        if (!isValidAction(&(node->state), possibleActions + i, player)) {
            nValidActions++;
        }
    }

    // Deep copy of parent 
    node->children = (struct Node*) malloc(nValidActions * sizeof(struct Node));
    
    nValidActions  = 0;
    for (int i = 0; i < nPossibleActions; ++i) {
        if (!isValidAction(&(node->state), possibleActions + i, player)) {
            ((node->children) + nValidActions)->nVisits = 0;
            ((node->children) + nValidActions)->score = 0;

            // Deep copy of board
            Board board = playAction(node->state, possibleActions + i);
            ((node->children) + nValidActions)->state.values = malloc(9 * sizeof(int));
            memcpy(((node->children) + nValidActions)->state.values, board.values, 9 * sizeof(int));
            ((node->children) + nValidActions)->state.nPlayers = board.nPlayers;
            ((node->children) + nValidActions)->parent = node;
            ((node->children) + nValidActions)->children = NULL;
            nValidActions++;
        }
    }
}


// TODO MC episode
int simulate_episode(Node* node, int player, char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board, Action*), int (*getScore) (Board*, int), void (*getPossibleActions)(Board, Action*), int (*getNumPossibleActions)(Board), void (*removeAction) (int, Action*, int)) {
    int initialPlayer = player;
    // Deep copy of board
    Board simulationBoard;
    simulationBoard.values = malloc(9 * sizeof(int));
    memcpy(simulationBoard.values, node->state.values, 9 * sizeof(int));
    simulationBoard.nPlayers = node->state.nPlayers;
    
    int nPossibleActions = getNumPossibleActions(simulationBoard);
    Action possibleActions[nPossibleActions];
    getPossibleActions(simulationBoard, possibleActions);

    // If the player is losing on a terminal node, avoid parent
    if (!(node->children) && (getScore(&(node->state), player) <= 0)) {
        node->parent->score = -__INT_MAX__;
        return 0;
    }

    // Random playout
    while (nPossibleActions > 0) {
        // Pick random action
        int random_action_idx = linear_congruential_random_generator() * (nPossibleActions);
        if (!isValidAction(&(node->state), &possibleActions[random_action_idx], player)) {
            // Play action 
            // Deep copy of board 
            Board board = playAction(simulationBoard, &possibleActions[random_action_idx]);
            memcpy(simulationBoard.values, board.values, 9 * sizeof(int));
            simulationBoard.nPlayers = board.nPlayers;

            // Remove action from possible actions
            nPossibleActions -= 2;
            removeAction(random_action_idx, possibleActions, nPossibleActions);
            printf("Board:\n[%d, %d, %d]\n[%d, %d, %d]\n[%d, %d, %d]\n", simulationBoard.values[0], simulationBoard.values[1],simulationBoard.values[2], simulationBoard.values[3], simulationBoard.values[4], simulationBoard.values[5], simulationBoard.values[6], simulationBoard.values[7], simulationBoard.values[8]);
            int score = getScore(&simulationBoard, player); // TODO add switch case
            if (score > 1) { // win
                if (player == initialPlayer) {
                    return 2;
                } else {
                    return 0;
                }  
            }
            player = -(player);
        }        
    }
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

// TODO MC game 
Board monte_carlo(Board board, int player, char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board, Action*), int (*getScore) (Board*, int), void (getPossibleActions)(Board, Action*), int (*getNumPossibleActions)(Board), void (*removeAction) (int, Action*, int)) {
    Node* node = malloc(sizeof(Node));
    node->nVisits = 0;
    node->score = 0;
    node->state = board;
    node->children = NULL;
    node->parent = NULL;
    set_linear_congruential_generator_seed(time(NULL));
    // while (node->nVisits == 0 | calc_UCB(node) < 2) { // TODO: add param for verif
    for (int i = 0; i < 20; ++i) {
        Node* selected_node = select_node(node);//malloc(sizeof(Node));
        printf("Selected node: [%d, %d, %d], [%d, %d, %d], [%d, %d, %d]\n", selected_node->state.values[0], selected_node->state.values[1], selected_node->state.values[2], selected_node->state.values[3], selected_node->state.values[4], selected_node->state.values[5], selected_node->state.values[6], selected_node->state.values[7], selected_node->state.values[8]);
        expand_leaf(selected_node, player, isValidAction, playAction, getPossibleActions, getNumPossibleActions);
        int score = simulate_episode(selected_node, player, isValidAction, playAction, getScore, getPossibleActions, getNumPossibleActions, removeAction);
        printf("Score: %d\n", score);
        backpropagate(selected_node, score);
    }
    // }

    Board ret = node->state;
    // TODO faire le free ailleurs!
    // free(node);
    return ret;
}
