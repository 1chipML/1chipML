
#ifndef ONECHIPML_MONTE_CARLO_RL
#define ONECHIPML_MONTE_CARLO_RL

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {  
    int values[2];
} Board;

typedef struct Node {
    unsigned nVisits;
    int score;
    unsigned nChildren;
    struct Node* parent;
    struct Node* children;
    Board state;
}  Node;

typedef struct {
    unsigned x_pos;
    unsigned player;
} Action;

typedef Board (*playActionType)(Board, Action*);
typedef char (*isValidActionType)(Board*, Action*, int);

// TODO: mettre en function pointers! 
Action* get_possible_actions(Board board);
int get_score(Board* board, int player);

Board monte_carlo(Board board, int player, char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board board, Action* action)); 
void expand_leaf(Node* node, int player,  char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board board, Action* action));

#ifdef __cplusplus
}
#endif

#endif // ONECHIPML_MONTE_CARLO_RL