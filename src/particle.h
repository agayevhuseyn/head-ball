#ifndef PARTICLE_H
#define PARTICLE_H


#include <raylib.h>


typedef struct {
    Vector2 pos;
    Vector2 dir;
    float velo;
    float life;
    float max_life;
    Color c;
    float size;
    int active;
} Particle;

void emit_particles(Particle *ps, int size, int needed,
                    Vector2 pos, Vector2 dir, float velo,
                    float life, Color c, float psize);
void update_particles(Particle *ps, int size, float dt);
void draw_particles(Particle *ps, int size);


#endif /* PARTICLE_H */
