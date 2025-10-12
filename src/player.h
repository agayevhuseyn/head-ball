#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "physics.h"

typedef struct Player Player;

#define PLAYER_LEFT_SIDE  0
#define PLAYER_RIGHT_SIDE 1

#define PLAYER_BRUNETTE 0
#define PLAYER_BLONDE   1
#define PLAYER_BLACK    2
#define PLAYER_BALD  3

struct Player {
    PObject p;
    Vector2 dir;
    float speed;
    float jmp_force;
    int side;
    int index;
};

void init_player(Player *player, int index, int side, Vector2 pos, float radius, float speed, float jmp_force);
void draw_player(Player *player);
void update_player(Player *player, float dt);

#endif
