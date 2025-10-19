#include "raylib.h"
#include "player.h"
#include "ball.h"

#define GAME_STATE_MENU  0
#define GAME_STATE_RUN   1
#define GAME_STATE_PAUSE 2

typedef struct {
    Player players[2];
    Ball ball;
    PObject borders[4];
    PObject bars[2];
    int state;
} Game;

void init_game(Game *game);
void draw_game(Game *game);
void update_game(Game *game, float dt);
