#ifndef PHYSICS_H
#define PHYSICS_H


#include <raylib.h>


typedef struct Circle Circle;

struct Circle {
    Vector2 pos;
    float radius;
};

typedef struct PObject PObject;

enum {
    POBJECT_CIR,
    POBJECT_REC
};

struct PObject {
    int type;
    union {
        Circle c;
        Rectangle r;
    } as;
    int isstatic;
    int bounce;
    int on_ground;
    float fric;
    float mass;
    float eloss;
    Vector2 velo;
};

#define pobject_staticrec(rect) (PObject) { \
    .type = POBJECT_REC, \
    .as.r = (rect), \
    .isstatic = true, \
    .mass = 0 \
}

#define ascir(p)  ((p).as.c)
#define asrec(p)  ((p).as.r)
#define ascirp(p) ((p)->as.c)
#define asrecp(p) ((p)->as.r)

#define iscir(p)  ((p).type == POBJECT_CIR)
#define isrec(p)  ((p).type == POBJECT_REC)
#define iscirp(p) ((p)->type == POBJECT_CIR)
#define isrecp(p) ((p)->type == POBJECT_REC)


Vector2 vec2unit(Vector2 v);
int check_cir_coll(Circle a, Circle b, Vector2 *normal, float *depth);
int check_cirrec_coll(Circle c, Rectangle r, Vector2 *normal, float *depth);
void handle_coll(PObject *a, PObject *b, float dt);


#endif /* PHYSICS_H */
