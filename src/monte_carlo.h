#ifndef ONECHIPML_MONTE_CARLO_RL
#define ONECHIPML_MONTE_CARLO_RL

#include <stdbool.h>
#include <stdint.h>

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
  int8_t* values;
  uint8_t nPlayers;
} Board;

typedef struct {
  uint8_t xPos : 6;
  uint8_t yPos : 6;
  int8_t player : 4;
} Action;

typedef void (*playActionType)(Board*, Action*);
typedef bool (*isValidActionType)(Board*, Action*, int);
typedef int (*getScoreType)(Board*, int);
typedef void (*getPossibleActionsType)(Board*, Action*);
typedef int (*getNumPossibleActionsType)(Board*);
typedef void (*removeActionType)(int, Action*, int);
typedef int (*getBoardSizeType)(void);
typedef bool (*isDoneType)(Board*);

typedef struct {
  // Methods specific to the game
  isValidActionType isValidAction;
  playActionType playAction;
  getScoreType getScore;
  getPossibleActionsType getPossibleActions;
  getNumPossibleActionsType getNumPossibleActions;
  removeActionType removeAction;
  getBoardSizeType getBoardSize;
  isDoneType isDone;
  int8_t lossValue;
  int8_t drawValue;
} Game;

typedef struct Node {
  uint16_t nVisits;
  uint16_t score;
  struct Node* parent;
  struct Node* children;
  Action action;
  uint8_t nChildren;
} Node;

mc_real calcUCB(Node* node);
Node* findMaxUCB(Node* children, unsigned nChildren);
Node* selectChildren(Node* node, Board* board, Game* game);
void expandLeaf(Node* node, Game game, Board* board);
int mcEpisode(Node* node, int player, Game* game, Board* board);
void backpropagate(Node* node, int score);
Action mcGame(Board board, int player, Game game, int minSim, int maxSim,
              mc_real goalValue);

#ifdef __cplusplus
}
#endif

#endif // ONECHIPML_MONTE_CARLO_RL
