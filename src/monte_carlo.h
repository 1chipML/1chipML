#ifndef ONECHIPML_MONTE_CARLO_RL
#define ONECHIPML_MONTE_CARLO_RL

#ifndef dft_real
    #define mc_real float
#endif

#ifndef M_PI
    #define UCB_MAX 1000
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {  
    int* values;
    int nPlayers;
} Board;

typedef struct {
    unsigned x_pos;
    unsigned y_pos;
    int player;
} Action;

typedef struct {
    // Methods specific to the game 
    char (*isValidAction)(Board*, Action*, int);
    Board (*playAction)(Board, Action*);
    int (*getScore) (Board*, int);
    void (*getPossibleActions)(Board, Action*);
    int (*getNumPossibleActions)(Board);
    void (*removeAction) (int, Action*, int);
    int (*getBoardSize) (void);
} Game;

typedef struct Node {
    unsigned nVisits;
    int score;
    struct Node* parent;
    struct Node* children;
    Board state;
    unsigned nChildren;
}  Node;

typedef Board (*playActionType)(Board, Action*);
typedef char (*isValidActionType)(Board*, Action*, int);
typedef int (*getScoreType) (Board*, int);
typedef void (*getPossibleActionsType)(Board, Action*);
typedef int (*getNumPossibleActionsType)(void);
typedef void (*removeActionType) (int, Action*, int);
typedef int (*getBoardSize) (void);

mc_real calc_UCB(Node* node);
Node* find_max_UCB(Node *children, unsigned nChildren);
Node* select_node(Node *node);
void expand_leaf(Node* node, int player,  Game game);
int mc_episode(Node* node, int player, Game game);
void backpropagate(Node *node, int score);
Board mc_game(Board board, int player, Game game, int minSim, int maxSim, mc_real goalValue); 

#ifdef __cplusplus
}
#endif

#endif // ONECHIPML_MONTE_CARLO_RL