
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
typedef int (*getScoreType) (Board*, int);
typedef Action* (getPossibleActionsType)(Board);

Board monte_carlo(Board board, int player, char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board, Action*), int (*getScore) (Board*, int), Action* (*getPossibleActions)(Board)); 
void expand_leaf(Node* node, int player,  char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board, Action*), Action* (*getPossibleActions)(Board));

#ifdef __cplusplus
}
#endif

#endif // ONECHIPML_MONTE_CARLO_RL