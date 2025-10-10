#include "game.h"
#include "macros.h"
#include "config.h"
#include <raymath.h>

#define JMP_FORCE -950
#define SPEED 400

void init_game(Game *game)
{
    init_ball(&game->ball, vec2(WIDTH / 2.0, 0), 32);
    init_player(&game->players[0], PLAYER_LEFT_SIDE,  vec2(100, 300), vec2(0, 0), 64, SPEED, JMP_FORCE);
    init_player(&game->players[1], PLAYER_RIGHT_SIDE, vec2(1180, 300), vec2(0, 0), 64, SPEED, JMP_FORCE);
}

void draw_game(Game *game)
{
    draw_player(&game->players[0]);
    draw_player(&game->players[1]);
    draw_ball(&game->ball);
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
    Vector2 diff = Vector2Subtract(b->pos, a->pos);
    float dist = Vector2Length(diff);
    float minDist = a->radius + b->radius;

    if (dist < minDist && dist > 0.0f) {
        // Normalized collision vector
        Vector2 normal = Vector2Scale(diff, 1.0f / dist);

        // Push each player away equally
        float overlap = minDist - dist;
        a->pos = Vector2Subtract(a->pos, Vector2Scale(normal, overlap * 0.5f));
        b->pos = Vector2Add(b->pos, Vector2Scale(normal, overlap * 0.5f));

        // Optional: simple velocity reflection for a bounce
        float relativeVelocity = (b->velo.x - a->velo.x) * normal.x + (b->velo.y - a->velo.y) * normal.y;
        if (relativeVelocity < 0) {
            float restitution = 0.3f; // small bounce
            Vector2 impulse = Vector2Scale(normal, -(1 + restitution) * relativeVelocity);
            a->velo = Vector2Subtract(a->velo, impulse);
            b->velo = Vector2Add(b->velo, impulse);
        }
    }
}

void update_game(Game *game, float dt)
{
    update_player(&game->players[0], dt);
    update_player(&game->players[1], dt);

    resolve_player_collision(&game->players[0], &game->players[1]);
    // players' collision to each other


    update_ball(&game->ball, dt, game->players);
}
