#include "ball.h"
#include "config.h"
#include <math.h>
#include "macros.h"
#include <raymath.h>
#include "game.h"
#include "particle.h"
#include <stdlib.h>
#include <string.h> /* memset */


#define PART_SIZE 256
static Particle ps[PART_SIZE] = {0};

void init_ball(Ball *ball, int type, Vector2 pos, float radius)
{
    memset(ps, 0, sizeof(ps));
    *ball = (Ball) {0};
    /* physics */
    ball->p.type = POBJECT_CIR;
    ball->p.isstatic = false;
    ball->p.bounce = true;
    ascir(ball->p).pos = pos;
    ascir(ball->p).radius = radius;
    ball->p.velo = vec2zero;

    ball->type = type;
    ball->rot = 0;
    ball->time_scale = 1.0f;

    /* friction and eloss must be between 0 and 1 (inclusively) */
    switch (ball->type) {
    case BALL_SOCCER:
    case BALL_BOMB:
        ball->p.fric = 0.01f; /* [0, 1] */
        ball->p.mass = 14.5f;
        ball->p.eloss = 0.25f; /* [0, 1] */
        break;
    case BALL_BOWLING:
        ball->p.fric = 0.05f;
        ball->p.mass = 100.0f;
        ball->p.eloss = 0.5f;
        break;
    case BALL_BEACH:
        ascir(ball->p).radius *= 2.0f;
        ball->p.fric = 0.01f;
        ball->p.mass = 5.0f;
        ball->p.eloss = 0.125f;
        break;
    case BALL_SLIME:
        ball->p.fric = 0.015f;
        ball->p.mass = 1.0f;
        ball->p.eloss = 0.0f;
        break;
    }
}

void draw_ball(Ball *ball, Texture2D tex)
{
    //DrawCircleV(ascir(ball->p).pos, ascir(ball->p).radius * 1.1f, BLACK);
    float radius = ascir(ball->p).radius;
    float height = ascir(ball->p).pos.y - GROUND;
    float shadow_intense = 1.0f - -height / GROUND;
    DrawEllipse(ascir(ball->p).pos.x, GROUND + 10, radius, radius/16, color(0, 0, 0, 120 * shadow_intense));
    draw_particles(ps, PART_SIZE);
    Rectangle src  = { BALL_SPRITE_SIZE * ball->type, 0, BALL_SPRITE_SIZE, BALL_SPRITE_SIZE };
    Rectangle dest = { ascir(ball->p).pos.x, ascir(ball->p).pos.y, radius * 2, radius * 2 };
    DrawTexturePro(tex, src, dest, vec2(radius, radius), ball->rot, WHITE);
    //draw_particles(ps, PART_SIZE);
}

void update_ball(Ball *ball, float dt)
{
    /* FIX */
    float min_mass = 5.0f;
    float max_mass = 100.0f;
    float mass_factor = (ball->p.mass - min_mass) / (max_mass - min_mass); // 0..1
    float acc = GRAVITY * (1.0f + mass_factor);  // 1x for lightest, 2x for heaviest
    /* TILL HERE */
    ball->p.velo.y += acc * dt * ball->time_scale;
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
    if (ball->type == BALL_BOMB) {
        Vector2 offset = vec2(
            (BALL_BOMB_ROPE_POS.x - BALL_SPRITE_SIZE / 2.0f) * (ascir(ball->p).radius / (BALL_SPRITE_SIZE / 2.0f)),
            (BALL_BOMB_ROPE_POS.y - BALL_SPRITE_SIZE / 2.0f) * (ascir(ball->p).radius / (BALL_SPRITE_SIZE / 2.0f))
        );
        float rad = ball->rot * DEG2RAD;
        float cosx = cosf(rad);
        float siny = sinf(rad);
        float rx = offset.x * cosx - offset.y * siny;
        float ry = offset.x * siny + offset.y * cosx;
        Vector2 actpos = vec2(ascir(ball->p).pos.x + rx, ascir(ball->p).pos.y + ry);
         emit_particles_rand(
            ps,                 /* Particle *ps,  */
            PART_SIZE,          /* int size,  */
            PARTICLE_SQUARE,    /* int type,  */
            3,                  /* int needed,  */
            actpos, /* Vector2 pos,  */
            vec2zero,         /* Vector2 dir,  */
            270,                /* float velo,  */
            0.4f,               /* float life,  */
            color(240, 175, 0, 255),             /* Color c,  */
            8 /* float psize  */
        );

        ball->bomb_time += dt;
        if (ball->bomb_time >= ball->bomb_max_time) {
            ball->bomb_time = 0;
            ball->bomb_max_time = 3;

            emit_particles_rand(
                ps,                 /* Particle *ps,  */
                PART_SIZE,          /* int size,  */
                PARTICLE_CIRCLE,    /* int type,  */
                72,                  /* int needed,  */
                ascir(ball->p).pos, /* Vector2 pos,  */
                vec2zero,         /* Vector2 dir,  */
                700,                /* float velo,  */
                1.2f,               /* float life,  */
                color(240, 140, 0, 255),             /* Color c,  */
                64 /* float psize  */
            );
        }
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
