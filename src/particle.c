#include "particle.h"
#include "macros.h"
#include <stdlib.h>
#include "physics.h"


void emit_particles(Particle *ps, int size, int type, int needed,
                    Vector2 pos, Vector2 dir, float velo,
                    float life, Color c, float psize)
{
    for (int i = 0; needed > 0 && i < size; i++) {
        if (ps[i].active)
            continue;

        ps[i].type = type;
        ps[i].pos  = pos;
        ps[i].dir  = vec2unit(dir);
        ps[i].velo = velo;
        ps[i].life = ps[i].max_life = life;
        ps[i].c = c;
        ps[i].size = psize;
        ps[i].active = true;

        needed--;
    }
}

void emit_particles_rand(Particle *ps, int size, int type, int needed,
                         Vector2 pos, Vector2 dir, float velo,
                         float life, Color c, float psize)
{
    for (int i = 0; i < needed; i++) {
        float angle = ((float)rand() / RAND_MAX) * 2.0f * PI;
        Vector2 rand_dir = vec2(cosf(angle), sinf(angle));
        emit_particles(
            ps,                 /* Particle *ps,  */
            size,          /* int size,  */
            type,    /* int type,  */
            1,                  /* int needed,  */
            pos, /* Vector2 pos,  */
            rand_dir,         /* Vector2 dir,  */
            rand() % (int)velo,                /* float velo,  */
            ((float)rand() / RAND_MAX) * life,               /* float life,  */
            color(c.r + rand() % 15, c.g + rand() % 15, c.b + rand() % 15 , c.a - rand() % 50),             /* Color c,  */
            psize /* float psize  */
        );
    }
}

void update_particles(Particle *ps, int size, float dt)
{
    for (int i = 0; i < size; i++) {
        if (!ps[i].active)
            continue;

        ps[i].pos.x += ps[i].dir.x * ps[i].velo * dt;
        ps[i].pos.y += ps[i].dir.y * ps[i].velo * dt;

        if ((ps[i].life -= dt) <= 0)
            ps[i].active = false;
    }
}

void draw_particles(Particle *ps, int size)
{
    for (int i = 0; i < size; i++) {
        if (!ps[i].active)
            continue;

        switch (ps[i].type) {
        case PARTICLE_CIRCLE:
            DrawCircleV(
                ps[i].pos,
                //vec2(ps[i].size, ps[i].size),
                ps[i].size * (ps[i].life / ps[i].max_life),
                color(
                    ps[i].c.r,
                    ps[i].c.g,
                    ps[i].c.b,
                    ps[i].c.a * (ps[i].life / ps[i].max_life)
                )
            );
            break;
        case PARTICLE_SQUARE:
            DrawRectanglePro(
                recv(ps[i].pos,
                //vec2(ps[i].size, ps[i].size),
                vec2(ps[i].size, ps[i].size)),
                vec2(ps[i].size / 2, ps[i].size / 2),
                0,
                color(
                    ps[i].c.r,
                    ps[i].c.g,
                    ps[i].c.b,
                    ps[i].c.a * (ps[i].life / ps[i].max_life)
                )
            );
            break;
        }
    }
}
