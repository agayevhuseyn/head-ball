#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include "physics.h"
#include "input.h"

#define PLAYER_SIDE_LEFT  0
#define PLAYER_SIDE_RIGHT 1

#define PLAYER_SPRITE_SIZE 32 /* full */
#define PLAYER_GAME_SIZE 64 /* radius */

enum {
    PLAYER_BRUNETTE,
    PLAYER_BLACK,
    PLAYER_BALD,
    PLAYER_SUMO,
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
    int is_bot;
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
    /* effects of supers */
    /* brunette */
    int dash_dir;
    /* black */
    int powershot;
    /* hacker */
    int revctrl;
    /* egg */
    int changing_size; /* 1 grow, -1 shrink, 0 none */
    float target_size;
    /* sumo */
    int smashing_ground;
    /* bomb */
    int stunned;
    /* breath */
    float breath;
    float height_diff;
} Player;

void init_player(Player *player, int index, int side, Vector2 pos, float radius, float speed, float jmp_force);
void draw_player(Player *player, Texture2D sheet);
void update_player(Player *player, PlayerInputResult ires, void *gameptr, float dt);

#endif
