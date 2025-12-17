#include "raylib.h"
#include "player.h"
#include "ball.h"
#include "input.h"
#include "sound.h"
#include "map.h"
#include "emote.h"

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_RUN,
    GAME_STATE_PAUSE
} GameState;

typedef enum {
    MENU_STATE_MAIN,
    MENU_STATE_PLAYMODE,
    MENU_STATE_GAMEPAD_PICK,
    MENU_STATE_BALLPICK,
    MENU_STATE_CHARPICK,
    MENU_STATE_SETTINGS
} MenuState;

#define GAME_ROUND_MAX_WAIT_TIME 1.5f

enum {
    GAME_MODE_SINGLE,
    GAME_MODE_MULTI,
    GAME_MODE_SIZE
};

typedef struct {
    Camera2D cam;
    Player players[2];
    PlayerControl controls[2];
    Ball ball;
    PObject borders[4];
    PObject bars[2];
    GameState game_state;
    MenuState menu_state;
    SoundManager sm;
    EmoteManager em;
    Map map;
    int game_mode; /* single or multiplayer */
    /* picking ball state */
    int ball_pick;
    /* picking character state */
    int char_picks[2];
    /* cleaning round */
    float wait_time;
    int cam_following_ball;
    int resetting_ball;
    /* shake */
    int shaking;
    float shake_time;
} Game;

void init_game(Game *game);
void draw_game(Game *game);
void update_game(Game *game, float dt);
