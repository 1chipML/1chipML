#ifndef ONECHIPML_MONTE_CARLO_RL
#define ONECHIPML_MONTE_CARLO_RL

#include <stdbool.h>

#ifndef mc_real
#define mc_real float
#endif

#ifndef UCB_MAX
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
  unsigned xPos;
  unsigned yPos;
  int player;
} Action;

typedef struct {
  // Methods specific to the game
  bool (*isValidAction)(Board*, Action*, int);
  Board (*playAction)(Board, Action*);
  int (*getScore)(Board*, int);
  void (*getPossibleActions)(Board, Action*);
  int (*getNumPossibleActions)(Board);
  void (*removeAction)(int, Action*, int);
  int (*getBoardSize)(void);
} Game;

typedef struct Node {
  unsigned nVisits;
  int score;
  struct Node* parent;
  struct Node* children;
  Board state;
  Action action;
  unsigned nChildren;
} Node;

typedef Board (*playActionType)(Board, Action*);
typedef bool (*isValidActionType)(Board*, Action*, int);
typedef int (*getScoreType)(Board*, int);
typedef void (*getPossibleActionsType)(Board, Action*);
typedef int (*getNumPossibleActionsType)(void);
typedef void (*removeActionType)(int, Action*, int);
typedef int (*getBoardSizeType)(void);

mc_real calcUCB(Node* node);
Node* findMaxUCB(Node* children, unsigned nChildren);
Node* selectNode(Node* node);
void expandLeaf(Node* node, Game game);
int mcEpisode(Node* node, int player, Game* game);
void backpropagate(Node* node, int score);
Action mcGame(Board board, int player, Game game, int minSim, int maxSim,
             mc_real goalValue);

#ifdef __cplusplus
}
#endif

#endif // ONECHIPML_MONTE_CARLO_RL
