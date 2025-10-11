#include "raylib.h"
#include "player.h"
#include "ball.h"

typedef struct Game Game;

struct Game {
    Player players[2];
    Ball ball;
    PObject borders[4];
    PObject bars[2];
};

void init_game(Game *game);
void draw_game(Game *game);
void update_game(Game *game, float dt);
