#ifndef ONECHIPML_MONTE_CARLO_RL
#define ONECHIPML_MONTE_CARLO_RL

typedef double mc_real;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {  
    int* values;
    int nPlayers;
} Board;

typedef struct Node {
    unsigned nVisits;
    int score;
    struct Node* parent;
    struct Node* children;
    Board state;
}  Node;

typedef struct {
    unsigned x_pos;
    unsigned y_pos;
    int player;
} Action;

typedef Board (*playActionType)(Board, Action*);
typedef char (*isValidActionType)(Board*, Action*, int);
typedef int (*getScoreType) (Board*, int);
typedef void (*getPossibleActionsType)(Board, Action*);
typedef int (*getNumPossibleActionsType)(void);
typedef void (*removeActionType) (int, Action*, int);


// TODO: mettre les fonctions dans une struct (genre la struct Board)
Board monte_carlo(Board board, int player, char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board, Action*), int (*getScore) (Board*, int), void (*getPossibleActions)(Board, Action*), int (*getNumPossibleActions)(Board), void (*removeAction) (int, Action*, int)); 
void expand_leaf(Node* node, int player,  char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board, Action*), void (*getPossibleActions)(Board, Action*), int (*getNumPossibleActions)(Board));
int simulate_episode(Node* node, int player, char (*isValidAction)(Board*, Action*, int), Board (*playAction)(Board, Action*), int (*getScore) (Board*, int), void (*getPossibleActions)(Board, Action*), int (*getNumPossibleActions)(Board), void (*removeAction) (int, Action*, int));

#ifdef __cplusplus
}
#endif

#endif // ONECHIPML_MONTE_CARLO_RL