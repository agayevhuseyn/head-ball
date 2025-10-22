#ifndef PARTICLE_H
#define PARTICLE_H


#include <raylib.h>

enum {
    PARTICLE_CIRCLE,
    PARTICLE_SQUARE
};

typedef struct {
    Vector2 pos;
    Vector2 dir;
    float velo;
    float life;
    float max_life;
    Color c;
    float size;
    int active;
    int type;
} Particle;

void emit_particles(Particle *ps, int size, int type, int needed,
                    Vector2 pos, Vector2 dir, float velo,
                    float life, Color c, float psize);

void emit_particles_rand(Particle *ps, int size, int type, int needed,
                         Vector2 pos, Vector2 dir, float velo,
                         float life, Color c, float psize);

void update_particles(Particle *ps, int size, float dt);
void draw_particles(Particle *ps, int size);


#endif /* PARTICLE_H */
