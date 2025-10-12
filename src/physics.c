#include "physics.h"
#include <raymath.h>
#include "macros.h"
#include <assert.h>


Vector2 vec2unit(Vector2 v)
{
    float l = sqrt(v.x * v.x + v.y * v.y);
    return l != 0 ? vec2(v.x / l, v.y / l) : vec2(0, 0);
}

int check_cir_coll(Circle a, Circle b, Vector2 *normal, float *depth)
{
    Vector2 diff = vec2(b.pos.x - a.pos.x, b.pos.y - a.pos.y);
    float dist = Vector2Length(diff);
    if (dist > a.radius + b.radius)
        return 0;

    if (normal)
        *normal = vec2unit(diff);

    if (depth)
        *depth = (a.radius + b.radius) - dist;

    return 1;
}

int check_cirrec_coll(Circle c, Rectangle r, Vector2 *normal, float *depth)
{
    Vector2 v = vec2(
        clamp(c.pos.x, r.x, r.x + r.width),
        clamp(c.pos.y, r.y, r.y + r.height)
    );
    Vector2 diff = vec2(
        c.pos.x - v.x,
        c.pos.y - v.y
    );
    float dist = Vector2Length(diff);
    if (dist > c.radius)
        return 0;

    if (normal)
        *normal = vec2unit(diff);

    if (depth)
        *depth = c.radius - dist;

    return 1;
}

static void resolve_cir_cir(PObject *a, PObject *b, Vector2 normal, float depth)
{
    if (a->isstatic && b->isstatic) return;

    // Push
    if (!a->isstatic && !b->isstatic) {
        ascirp(a).pos.x -= normal.x * (depth * 0.5f);
        ascirp(a).pos.y -= normal.y * (depth * 0.5f);
        ascirp(b).pos.x += normal.x * (depth * 0.5f);
        ascirp(b).pos.y += normal.y * (depth * 0.5f);
    } else if (!a->isstatic) {
        ascirp(a).pos.x -= normal.x * depth;
        ascirp(a).pos.y -= normal.y * depth;
    } else if (!b->isstatic) {
        ascirp(b).pos.x += normal.x * depth;
        ascirp(b).pos.y += normal.y * depth;
    }

    // Velocity resolution (elastic collision)
    if (!a->isstatic && !b->isstatic) {
        Vector2 rel_velo = vec2(b->velo.x - a->velo.x, b->velo.y - a->velo.y);
        float velo_along_normal = rel_velo.x*normal.x + rel_velo.y*normal.y;

        if (velo_along_normal > 0) return;

        float e = 0.8f; // restitution (bounciness)
        float j = -(1 + e) * velo_along_normal / (1/a->mass + 1/b->mass);

        Vector2 impulse = vec2(normal.x*j, normal.y*j);
        a->velo.x -= impulse.x / a->mass;
        a->velo.y -= impulse.y / a->mass;
        b->velo.x += impulse.x / b->mass;
        b->velo.y += impulse.y / b->mass;
    } else if (!a->isstatic) {
        float j = -(1 + 0.8f) * (a->velo.x*normal.x + a->velo.y*normal.y);
        j /= (1/a->mass);
        a->velo.x += normal.x*j / a->mass;
        a->velo.y += normal.y*j / a->mass;
    } else if (!b->isstatic) {
        float j = -(1 + 0.8f) * (b->velo.x*normal.x + b->velo.y*normal.y);
        j /= (1/b->mass);
        b->velo.x += normal.x*j / b->mass;
        b->velo.y += normal.y*j / b->mass;
    }
}

static void resolve_cir_rec(PObject *cobj, PObject *robj, Vector2 normal, float depth)
{
    if (cobj->isstatic) return;

    // Push
    ascirp(cobj).pos.x += normal.x * depth;
    ascirp(cobj).pos.y += normal.y * depth;

    int on_wall = fabsf(normal.x) > 0.5f;
    int on_ground = normal.y < -0.5f;
    int floor = normal.y > 0.5f;
    if (on_ground)
        cobj->on_ground = 1;
    //bool on_ground = normal.y < -0.5f;   // assuming up is negative Y

    // Reflect velocity
    float vel_along_normal = cobj->velo.x * normal.x + cobj->velo.y * normal.y;
    if (vel_along_normal < 0) {
        float e = 1.0f - clamp(cobj->eloss, 0.0f, 1.0f);
        if (cobj->bounce) {
            cobj->velo.x -= (1 + e) * vel_along_normal * normal.x;
            cobj->velo.y -= (1 + e) * vel_along_normal * normal.y;
        } else {
            if (!on_wall) {
                cobj->velo.y = 0;
            } else if (!floor) {
                /* TODO */
                cobj->velo.x -= (1 + e) * vel_along_normal * normal.x;
                cobj->velo.y -= (1 + e) * vel_along_normal * normal.y;
            } else {
            }
        }
    }

    Vector2 tangent = vec2(-normal.y, normal.x);
    float vel_along_tangent = cobj->velo.x*tangent.x + cobj->velo.y*tangent.y;

    float mu = cobj->fric;
    cobj->velo.x -= vel_along_tangent * mu * tangent.x;
    cobj->velo.y -= vel_along_tangent * mu * tangent.y;
}

void handle_coll(PObject *a, PObject *b, float dt)
{
    Vector2 normal;
    float depth;
    int collided = 0;

    if (iscirp(a) && iscirp(b)) {
        collided = check_cir_coll(ascirp(a), ascirp(b), &normal, &depth);
        if (collided)
            resolve_cir_cir(a, b, normal, depth);
    } else if (iscirp(a) && isrecp(b)) {
        collided = check_cirrec_coll(ascirp(a), asrecp(b), &normal, &depth);
        if (collided)
            resolve_cir_rec(a, b, normal, depth);
    } else {
        assert(0 && "Circle must be first argument");
    }

    if (!collided) return;
    assert(!a->isstatic && "First argument must be dynamic");
}
