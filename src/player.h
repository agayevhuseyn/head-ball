#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "physics.h"

#define PLAYER_SIDE_LEFT  0
#define PLAYER_SIDE_RIGHT 1

#define PLAYER_SPRITE_SIZE 32
#define PLAYER_GAME_SIZE 64

enum {
    PLAYER_BRUNETTE,
    PLAYER_BLACK,
    PLAYER_BALD,
    PLAYER_ASIAN,
    PLAYER_MATRIX,
    PLAYER_HACKER,
    PLAYER_PANDA,
    PLAYER_ALIEN,
    PLAYER_SIZE, /* number of players */
};

typedef struct {
    PObject p;
    Vector2 dir;
    float speed;
    float jmp_force;
    int side;
    int index;
    Vector2 hboffset;
    float hbradius;
    /* super */
    struct {
        int active;
        int charged;
        int being_used;
        float chr_time;
        float maxchr_time;
        float use_time;
        float maxuse_time;
    } super;
    /* side effect of opponent */
    int reverse;
} Player;

Texture2D get_player_tex();
void init_player(Player *player, int index, int side, Vector2 pos, float radius, float speed, float jmp_force);
void draw_player(Player *player, Texture2D sheet);
void update_player(Player *player, void *gameptr, float dt);

#endif
