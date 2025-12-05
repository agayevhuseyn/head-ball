#ifndef BALL_H
#define BALL_H

#include "raylib.h"
#include "player.h"
#include "physics.h"

#define BALL_MAX_STOP_TIME 1.0f

#define BALL_SPRITE_SIZE 16 /* full */
#define BALL_GAME_SIZE 32 /* radius */
#define BALL_BOMB_ROPE_POS vec2(1, 4) /* in sprite */
// 1, 4
#define SHAKE_MAX_TIME 0.35f

enum {
    BALL_SOCCER,
    BALL_BOMB,
    BALL_BOWLING,
    BALL_BEACH,
    BALL_SLIME,
    BALL_SIZE
};

typedef struct {
    PObject p;
    int type;
    float rot;
    /* supers from players */
    float time_scale;
    int hitleft_trail;
    /* ball stop */
    float stop_time;
    /* bomb */
    float bomb_time;
    float bomb_max_time;
} Ball;

void init_ball(Ball *ball, int type, Vector2 pos, float radius);
void draw_ball(Ball *ball, Texture2D tex);
void update_ball(Ball *ball, void *gameptr, float dt);

#endif
