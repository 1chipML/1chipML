typedef struct {  
    int values[2];
} Board;

typedef struct {
    unsigned x_pos;
    unsigned player;
} Action;

Action* get_possible_actions(Board board);
int get_score(Board* board, int player);
Board play_action(Board board, Action* action);
char is_valid_action(Board* board, Action* action, int player);
Board monte_carlo(Board board, int player);