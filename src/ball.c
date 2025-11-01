#include "ball.h"
#include "config.h"
#include <math.h>
#include "macros.h"
#include <raymath.h>
#include "game.h"
#include "particle.h"
#include <stdlib.h>


#define PART_SIZE 256
static Particle ps[PART_SIZE] = {0};

void init_ball(Ball *ball, Vector2 pos, float radius)
{
    *ball = (Ball) {0};
    /* physics */
    ball->p.type = POBJECT_CIR;
    ball->p.isstatic = false;
    ball->p.bounce = true;
    ball->p.fric = 0.01f; /* [0, 1] */
    ball->p.mass = 14.5f;
    ball->p.eloss = 0.25f; /* [0, 1] */
    ascir(ball->p).pos = pos;
    ascir(ball->p).radius = radius;
    ball->p.velo = vec2zero;

    ball->rot = 0;
    ball->time_scale = 1.0f;
}

void draw_ball(Ball *ball, Texture2D tex)
{
    //DrawCircleV(ascir(ball->p).pos, ascir(ball->p).radius * 1.1f, BLACK);
    draw_particles(ps, PART_SIZE);
    Rectangle src  = { 0, 0, tex.width, tex.height };
    Rectangle dest = { ascir(ball->p).pos.x, ascir(ball->p).pos.y, ascir(ball->p).radius * 2, ascir(ball->p).radius * 2 };
    DrawTexturePro(tex, src, dest, vec2(ascir(ball->p).radius, ascir(ball->p).radius), ball->rot, WHITE);
}

void update_ball(Ball *ball, float dt)
{
    ball->p.velo.y += GRAVITY * dt * ball->time_scale;
    ascir(ball->p).pos.x += ball->p.velo.x * dt * ball->time_scale;
    ascir(ball->p).pos.y += ball->p.velo.y * dt * ball->time_scale;

    ball->rot += (ball->p.velo.x / ascir(ball->p).radius) * RAD2DEG * dt * ball->time_scale; 

    if (ball->hitleft_trail > 0) {
        emit_particles(
            ps,                 /* Particle *ps,  */
            PART_SIZE,          /* int size,  */
            PARTICLE_CIRCLE,    /* int type,  */
            1,                  /* int needed,  */
            ascir(ball->p).pos, /* Vector2 pos,  */
            vec2zero,         /* Vector2 dir,  */
            100,                /* float velo,  */
            0.3f,               /* float life,  */
            ORANGE,             /* Color c,  */
            ascir(ball->p).radius   /* float psize  */
        );
    }
    if (ball->time_scale < 1.0f) {
        emit_particles_rand(
            ps,                 /* Particle *ps,  */
            PART_SIZE,          /* int size,  */
            PARTICLE_CIRCLE,    /* int type,  */
            1,                  /* int needed,  */
            ascir(ball->p).pos, /* Vector2 pos,  */
            vec2zero,         /* Vector2 dir,  */
            150,                /* float velo,  */
            0.6f,               /* float life,  */
            color(63, 63, 116, 255),             /* Color c,  */
            ascir(ball->p).radius /* float psize  */
        );
    }

    update_particles(ps, PART_SIZE, dt);
    
#if DEBUG
    if (IsMouseButtonDown(0)) {
        ascir(ball->p).pos.x = lerp(ascir(ball->p).pos.x, GetMouseX(), dt * 5);
        ascir(ball->p).pos.y = lerp(ascir(ball->p).pos.y, GetMouseY(), dt * 5);
        ball->p.velo = vec2zero;
    }
#endif
}
