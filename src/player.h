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
#define PLAYER_LAST PLAYER_BALD
#define PLAYER_SIZE (PLAYER_LAST + 1)

struct Player {
    PObject p;
    Vector2 dir;
    float speed;
    float jmp_force;
    int side;
    int index;
};

Texture2D get_player_tex();
void init_player(Player *player, int index, int side, Vector2 pos, float radius, float speed, float jmp_force);
void draw_player(Player *player, Texture2D sheet);
void update_player(Player *player, float dt);

#endif
