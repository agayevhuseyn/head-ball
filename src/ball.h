#ifndef BALL_H
#define BALL_H

#include "raylib.h"
#include "player.h"
#include "physics.h"

typedef struct Ball Ball;

struct Ball {
    PObject p;
    float rot;
};

void init_ball(Ball *ball, Vector2 pos, float radius);
void draw_ball(Ball *ball, Texture2D tex);
void update_ball(Ball *ball, float dt);

#endif
