#include "ball.h"
#include "config.h"
#include <math.h>
#include "macros.h"
#include <raymath.h>
#include "game.h"

#define FRICTION 0.99f

static Texture2D ball_tex;

void init_ball(Ball *ball, Vector2 pos, float radius)
{
    *ball = (Ball) {0};
    /* physics */
    ball->p.type = POBJECT_CIR;
    ball->p.isstatic = false;
    ball->p.bounce = true;
    ball->p.fric = 0.02f; /* TODO */
    ball->p.mass = 10;
    ascir(ball->p).pos = pos;
    ascir(ball->p).radius = radius;
    ball->p.velo = vec2(0, 0);

    ball_tex = LoadTexture("assets/ball.png");
}

void draw_ball(Ball *ball)
{
    DrawCircleV(ascir(ball->p).pos, ascir(ball->p).radius * 1.1f, BLACK);
    Rectangle src  = { 0, 0, ball_tex.width, ball_tex.height };
    Rectangle dest = { ascir(ball->p).pos.x, ascir(ball->p).pos.y, ascir(ball->p).radius * 2, ascir(ball->p).radius * 2 };
    DrawTexturePro(ball_tex, src, dest, vec2(ascir(ball->p).radius, ascir(ball->p).radius), ball->rot, WHITE);
}

void update_ball(Ball *ball, float dt)
{
    // Gravity + motion
    ball->p.velo.y += GRAVITY * dt;
    ascir(ball->p).pos.x += ball->p.velo.x * dt;
    ascir(ball->p).pos.y += ball->p.velo.y * dt;

    // Ground collision
    //if (ascir(ball->p).pos.y + ascir(ball->p).radius >= GROUND) {
    //    ascir(ball->p).pos.y = GROUND - ascir(ball->p).radius;
    //    ball->p.velo.y *= -0.75f; // energy loss
    //    ball->p.velo.x *= FRICTION;  // friction
    //}
    ball->rot += (ball->p.velo.x / ascir(ball->p).radius) * RAD2DEG * dt;
    if (IsMouseButtonDown(0)) {
        ascir(ball->p).pos = GetMousePosition();
    }
}
