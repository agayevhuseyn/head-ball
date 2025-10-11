#include "game.h"
#include "macros.h"
#include "config.h"
#include <raymath.h>

#define JMP_FORCE -950
#define SPEED 400

/* BAR */
#define BAR_WIDTH  156
#define BAR_HEIGHT 32
#define BAR_POS_Y  328

#define BOR_THICK 32

static Texture2D bg_tex;

static void draw_recs(PObject *ps)
{
    for (int i = 0; i < 4; i++) {
        DrawRectangleRec(asrec(ps[i]), RED);
    }
}

void init_game(Game *game)
{
    bg_tex = LoadTexture("assets/bg.png");
    /* ball */
    init_ball(&game->ball, vec2(WIDTH / 2.0, 128), 32);
    /* players */
    init_player(&game->players[0], PLAYER_LEFT_SIDE,  vec2(100, 300),  64, SPEED, JMP_FORCE);
    init_player(&game->players[1], PLAYER_RIGHT_SIDE, vec2(1180, 300), 64, SPEED, JMP_FORCE);
    /* bars */
    game->bars[0] = pobject_staticrec(rec(0, BAR_POS_Y, BAR_WIDTH, BAR_HEIGHT));
    game->bars[1] = pobject_staticrec(rec(WIDTH - BAR_WIDTH, BAR_POS_Y, BAR_WIDTH, BAR_HEIGHT));
    /* borders */
    game->borders[0] = pobject_staticrec(rec(0, 0 - BOR_THICK, WIDTH, BOR_THICK));
    game->borders[1] = pobject_staticrec(rec(0, GROUND, WIDTH, BOR_THICK));
    game->borders[2] = pobject_staticrec(rec(0 - BOR_THICK, 0, BOR_THICK, GROUND));
    game->borders[3] = pobject_staticrec(rec(WIDTH, 0, BOR_THICK, GROUND));
}

void draw_game(Game *game)
{
    DrawTextureEx(bg_tex, vec2(0, 0), 0, SCALE, WHITE);
    //draw_bars(game);
    draw_player(&game->players[0]);
    draw_player(&game->players[1]);
    draw_ball(&game->ball);
    //draw_recs(game->borders);
}

void resolve_player_collision(Player *a, Player *b)
{
    /*
    Vector2 diff = Vector2Subtract(b->pos, a->pos);
    float dist = Vector2Length(diff);
    float minDist = a->radius + b->radius;

    if (dist < minDist && dist > 0.0f) {
        Vector2 normal = Vector2Scale(diff, 1.0f / dist);

        // Only separate horizontally
        a->pos.x -= normal.x * (minDist - dist) * 0.5f;
        b->pos.x += normal.x * (minDist - dist) * 0.5f;

        // Optional: apply tiny horizontal velocity to simulate push
        float push = 50.0f;
        a->velo.x -= normal.x * push * 0.5f;
        b->velo.x += normal.x * push * 0.5f;
    }
    */
    Vector2 diff = Vector2Subtract(ascir(b->p).pos, ascir(a->p).pos);
    float dist = Vector2Length(diff);
    float minDist = ascir(a->p).radius + ascir(b->p).radius;

    if (dist < minDist && dist > 0.0f) {
        // Normalized collision vector
        Vector2 normal = Vector2Scale(diff, 1.0f / dist);

        // Push each player away equally
        float overlap = minDist - dist;
        ascir(a->p).pos = Vector2Subtract(ascir(a->p).pos, Vector2Scale(normal, overlap * 0.5f));
        ascir(b->p).pos = Vector2Add(ascir(b->p).pos, Vector2Scale(normal, overlap * 0.5f));

        // Optional: simple velocity reflection for a bounce
        float relativeVelocity = (b->p.velo.x - a->p.velo.x) * normal.x + (b->p.velo.y - a->p.velo.y) * normal.y;
        if (relativeVelocity < 0) {
            float restitution = 0.3f; // small bounce
            Vector2 impulse = Vector2Scale(normal, -(1 + restitution) * relativeVelocity);
            a->p.velo = Vector2Subtract(a->p.velo, impulse);
            b->p.velo = Vector2Add(b->p.velo, impulse);
        }
    }
}

void update_game(Game *game, float dt)
{
    Player *a  = &game->players[0];
    Player *b  = &game->players[1];
    Ball *ball = &game->ball;
    update_player(a, dt);
    update_player(b, dt);

    update_ball(&game->ball, dt);

    handle_coll(&a->p, &b->p, dt);
    handle_coll(&ball->p, &a->p, dt);
    handle_coll(&ball->p, &b->p, dt);

    a->p.on_ground = b->p.on_ground = 0;
    for (int i = 0; i < 4; i++) {
        handle_coll(&ball->p, &game->borders[i], dt);
        handle_coll(&a->p, &game->borders[i], dt);
        handle_coll(&b->p, &game->borders[i], dt);
    }
    for (int i = 0; i < 2; i++) {
        handle_coll(&ball->p, &game->bars[i], dt);
        handle_coll(&a->p, &game->bars[i], dt);
        handle_coll(&b->p, &game->bars[i], dt);
    }
}
