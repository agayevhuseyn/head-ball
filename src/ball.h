#ifndef BALL_H
#define BALL_H

#include "raylib.h"
#include "player.h"

typedef struct Ball Ball;

struct Ball {
    Vector2 pos;
    float radius;
    Vector2 velo;
};

void init_ball(Ball* ball, Vector2 pos, float radius);
void draw_ball(Ball* ball);
void update_ball(Ball* ball, float dt, const Player players[2]);

#endif
