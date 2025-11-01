#include "raylib.h"
#include "player.h"
#include "ball.h"
#include "input.h"

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_RUN,
    GAME_STATE_PAUSE
} GameState;

typedef enum {
    MENU_STATE_MAIN,
    MENU_STATE_PICK,
    MENU_STATE_SETTINGS
} MenuState;

#define GAME_ROUND_MAX_WAIT_TIME 1.5f

typedef struct {
    Camera2D cam;
    Player players[2];
    PlayerControl controls[2];
    Ball ball;
    PObject borders[4];
    PObject bars[2];
    GameState game_state;
    MenuState menu_state;
    /* cleaning round */
    float wait_time;
    int cam_following_ball;
    int resetting_ball;
} Game;

void init_game(Game *game);
void draw_game(Game *game);
void update_game(Game *game, float dt);
