#include "raylib.h"
#include "player.h"
#include "ball.h"
#include "input.h"

#define GAME_STATE_MENU  0
#define GAME_STATE_RUN   1
#define GAME_STATE_PAUSE 2

#define GAME_ROUND_MAX_WAIT_TIME 1.5f

typedef struct {
    Camera2D cam;
    Player players[2];
    PlayerControl controls[2];
    Ball ball;
    PObject borders[4];
    PObject bars[2];
    int state;
    /* cleaning round */
    float wait_time;
    int cam_following_ball;
    int resetting_ball;
} Game;

void init_game(Game *game);
void draw_game(Game *game);
void update_game(Game *game, float dt);
