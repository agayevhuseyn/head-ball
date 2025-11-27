#ifndef BALL_H
#define BALL_H

#include "raylib.h"
#include "player.h"
#include "physics.h"

#define BALL_MAX_STOP_TIME 1.0f

typedef struct {
    PObject p;
    float rot;
    /* supers from players */
    float time_scale;
    int hitleft_trail;
    /* ball stop */
    float stop_time;
} Ball;

void init_ball(Ball *ball, Vector2 pos, float radius);
void draw_ball(Ball *ball, Texture2D tex);
void update_ball(Ball *ball, float dt);

#endif
