#include "monte_carlo.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int calc_UCB(Node* node) {
    if(node->nVisits == 0) {
        return __INT_MAX__; 
    }
    return node->score / node->nVisits;
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
    while(curr_node->children) { 
        curr_node = find_max_UCB(curr_node->children);
        if (calc_UCB(curr_node) == __INT_MAX__) {
            return curr_node;
        }
    }
    return curr_node;
}

void remove_action(int randomActionIdx, Action* possibleActions) {
    for (int i = randomActionIdx; i < (sizeof(possibleActions)/sizeof(Action)); i++) {
        possibleActions[i] = possibleActions[i + 1];
    }
}

void expand_leaf(Node* node, int player, char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board, Action*), Action* (*getPossibleActions)(Board)) {
    Action* possibleActions = getPossibleActions(node->state);
    int nValidActions = 0;

    // Check if action is valid, if it is, add to children, if not, don't add
    for (int i = 0; i < 4; i++) {
        if (isValidAction(&(node->state), possibleActions + i, player)) {
            nValidActions++;
        }
    }

    node->children = (struct Node*) malloc(nValidActions * sizeof(struct Node));
    
    nValidActions  = 0;
    for (int i = 0; i < 4; i++) {
        if (isValidAction(&(node->state), possibleActions + i, player)) {
            (node->children + nValidActions)->nVisits = 0;
            (node->children + nValidActions)->score = 0;
            (node->children + nValidActions)->nChildren = 0;
            (node->children + nValidActions)->state = playAction(node->state, possibleActions + i);
            (node->children + nValidActions)->parent = node;
            nValidActions++;
        }
    }
}

int simulate_episode(Node* node, int player, char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board, Action*), int (*getScore) (Board*, int), Action* (*getPossibleActions)(Board)) {
    Board simulationBoard = node->state;
    Action* possibleActions = getPossibleActions(simulationBoard);
    int nPossibleActions = 2; // Change for size of possible actions
    Node* children = node->children;

    // If the player is losing on a terminal node, avoid parent
    if (!children & (getScore(&(node->state), player) <= 0)) {
        node->parent->score = -__INT_MAX__;
        return 0;
    }

    // Random playout
    while (nPossibleActions > 0) {
        // Pick random action
        int random_action_idx = rand() % (nPossibleActions + 1);
        if (isValidAction(&(node->state), &possibleActions[random_action_idx], player)) {
            // Play action 
            simulationBoard = playAction(simulationBoard, &possibleActions[random_action_idx]);
        }
        // Remove action from possible actions
        remove_action(random_action_idx, possibleActions);
        nPossibleActions--;
    }
    // Get score and return it 
    int score = getScore(&simulationBoard, player);
    if (score < 0) { // loss
        return 0;
    } else if (score > 0) { // win
        return 2;
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

Board monte_carlo(Board board, int player, char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board, Action*), int (*getScore) (Board*, int), Action* (*getPossibleActions)(Board)) {
    Node* node = malloc(sizeof(Node));
    node->nVisits = 0;
    node->score = 0;
    node->state = board;

    while (node->nVisits == 0 | calc_UCB(node) < 2) {
        Node* selected_node = select_node(node);
        expand_leaf(selected_node, player, isValidAction, playAction, getPossibleActions);
        int score = simulate_episode(node, player, isValidAction, playAction, getScore, getPossibleActions);
        backpropagate(node, score);
    }

    Board ret = node->state;
    free(node);
    return ret;
}
