#include "monte_carlo.h"
#include <math.h>
#include <stdlib.h>

typedef struct Node {
    unsigned nVisits;
    int score;
    unsigned nChildren;
    struct Node* parent;
    struct Node* children;
    Board state;
}  Node;

int get_score(Board* board, int player) {
    if (board->values[0] == 0 & board->values[1] == 0) {
        return 1;
    }
    if (board->values[0] == 1) {
        return 0;
    }
    if (board->values[1] == 1) {
        return 2;
    }
}

char is_valid_action(Board* board, Action* action, int player) {
    if (action->player == player & board->values[action->x_pos] == 0) {
        return 1;
    }
    return 0;
}

Action* get_possible_actions(Board board) {
    Action* possibleActions = malloc(4 * sizeof(Action));
    Action action1;
    action1.x_pos = 0;
    action1.player = 0;
    possibleActions[0] = action1;

    action1.x_pos = 1;
    action1.player = 0;
    possibleActions[1] = action1;

    action1.x_pos = 0;
    action1.player = 1;
    possibleActions[2] = action1;

    action1.x_pos = 1;
    action1.player = 1;
    possibleActions[3] = action1;

    return possibleActions;
}

Board play_action(Board board, Action* action) {
    Board childBoard = board;
    childBoard.values[action->x_pos] = action->player;
    return childBoard;
}

int calc_UCB(Node *node) {
    if(node->nVisits == 0) {
        return __INT_MAX__; 
    }
    return node->score / node->nVisits; // (node->score / node->nVisits) + sqrt(2 * log(node->parent->nVisits) / node->nVisits);
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

void expand_leaf(Node* node, int player) {
    Action* possibleActions = get_possible_actions(node->state);
    int nValidActions = 0;

    // Check if action is valid, if it is, add to children, if not, don't add
    for (int i = 0; i < 4; i++) {
        if (is_valid_action(&(node->state), possibleActions + i, player)) {
            nValidActions++;
        }
    }

    node->children = (struct Node*) malloc(nValidActions * sizeof(struct Node));
    
    nValidActions  = 0;
    for (int i = 0; i < 4; i++) {
        if (is_valid_action(&(node->state), possibleActions + i, player)) {
            (node->children + nValidActions)->nVisits = 0;
            (node->children + nValidActions)->score = 0;
            (node->children + nValidActions)->nChildren = 0;
            (node->children + nValidActions)->state = play_action(node->state, possibleActions + i);
            (node->children + nValidActions)->parent = node;
            nValidActions++;
        }
    }
}

int simulate_episode(Node* node, int player) {
    Board simulationBoard = node->state;
    Action* possibleActions = get_possible_actions(simulationBoard);
    int nPossibleActions = 2; // Change for size of possible actions
    Node* children = node->children;

    // If the player is losing on a terminal node, avoid parent
    if (!children & (get_score(&(node->state), player) <= 0)) {
        node->parent->score = -__INT_MAX__;
        return 0;
    }

    // Random playout
    while (nPossibleActions > 0) {
        // Pick random action
        int random_action_idx = rand() % (nPossibleActions + 1);
        if (is_valid_action(&(node->state), &possibleActions[random_action_idx], player)) {
            // Play action
            simulationBoard = play_action(simulationBoard, &possibleActions[random_action_idx]);
        }
        // Remove action from possible actions
        remove_action(random_action_idx, possibleActions);
        nPossibleActions--;
    }
    // Get score and return it 
    int score = get_score(&simulationBoard, player);
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

Board monte_carlo(Board board, int player) {
    Node* node = malloc(sizeof(Node));
    node->nVisits = 0;
    node->score = 0;
    node->state = board;

    while (node->nVisits == 0 | calc_UCB(node) < 2) {
        Node* selected_node = select_node(node);
        expand_leaf(selected_node, player);
        int score = simulate_episode(node, player);
        backpropagate(node, score);
    }

    Board ret = node->state;
    free(node);
    return ret;
}
