#include "monte_carlo.h"

struct Node {
    struct Node *children;
    int value;
    int currentUCB; 
};

void calc_reward() {}

void monte_carlo(struct Node *node) {
    select_node(node);
    expand_leaf(node);
    simulate_episode();
    backpropagate();
}

void select_node(struct Node *node) {

}

void expand_leaf(struct Node *node) {}

void simulate_episode() {}

void backpropagate() {}