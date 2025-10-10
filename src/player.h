#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct Player Player;

#define PLAYER_LEFT_SIDE  0
#define PLAYER_RIGHT_SIDE 1

struct Player {
    Vector2 pos;
    Vector2 dir;
    Vector2 velo;
    float radius;
    float speed;
    float jmp_force;
    bool can_jump;
    int side;
};

void init_player(Player *player, int side, Vector2 pos, Vector2 dir,
                 float radius, float speed, float jmp_force);
void draw_player(Player *player);
void update_player(Player *player, float dt);

#endif
