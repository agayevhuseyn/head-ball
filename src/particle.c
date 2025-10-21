#include "particle.h"
#include "macros.h"


void emit_particles(Particle *ps, int size, int needed,
                    Vector2 pos, Vector2 dir, float velo,
                    float life, Color c, float psize)
{
    for (int i = 0; needed > 0 && i < size; i++) {
        if (ps[i].active)
            continue;

        ps[i].pos  = pos;
        ps[i].dir  = dir;
        ps[i].velo = velo;
        ps[i].life = ps[i].max_life = life;
        ps[i].c = c;
        ps[i].size = psize;
        ps[i].active = true;

        needed--;
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
    }
}
