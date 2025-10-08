#include "ball.h"
#include "config.h"
#include <math.h>
#include "macros.h"
#include <raymath.h>

#define FRICTION 0.99f

void init_ball(Ball* ball, Vector2 pos, float radius)
{
    *ball = (Ball) {0};
    ball->pos = pos;
    ball->radius = radius;
    ball->velo = vec2(0, 0);
}

void draw_ball(Ball* ball)
{
    DrawCircleV(ball->pos, ball->radius, RAYWHITE);
}

Vector2 normalize(Vector2 vec) {
    float length = sqrt(vec.x * vec.x + vec.y * vec.y);
    return length != 0 ? vec2(vec.x / length, vec.y / length) : vec2(0, 0);
}

Vector2 check_circle_collision(Vector2 a, Vector2 b, float ar, float br)
{
    float distance = sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
    if (distance > ar + br)
        return vec2(0, 0);
    return normalize(vec2(b.x - a.x, b.y - a.y));
}

void update_ball(Ball* ball, float dt, const Player players[2])
{
    // Gravity + motion
    ball->velo.y += GRAVITY * dt;
    ball->pos.x  += ball->velo.x * dt;
    ball->pos.y  += ball->velo.y * dt;

    // Ground collision
    if (ball->pos.y + ball->radius >= HEIGHT) {
        ball->pos.y = HEIGHT - ball->radius;
        ball->velo.y *= -0.75f; // energy loss
        ball->velo.x *= FRICTION;  // friction
    }
    // Floor coliison
    if (ball->pos.y - ball->radius <= 0) {
        ball->pos.y = ball->radius;
        ball->velo.y *= -0.75f;
        ball->velo.x *= FRICTION;
    }

    // Wall collisions
    if (ball->pos.x - ball->radius <= 0) {
        ball->pos.x = ball->radius;
        ball->velo.x *= -0.9f;
    } else if (ball->pos.x + ball->radius >= WIDTH) {
        ball->pos.x = WIDTH - ball->radius;
        ball->velo.x *= -0.9f;
    }

    // Player collisions
    for (int i = 0; i < 2; i++) {
        Vector2 diff = Vector2Subtract(ball->pos, players[i].pos);
        float dist = Vector2Length(diff);
        float minDist = ball->radius + players[i].radius;

        if (dist < minDist && dist > 0.0f) {
            // Normalize
            Vector2 normal = Vector2Scale(diff, 1.0f / dist);

            // Separate
            float overlap = minDist - dist;
            ball->pos = Vector2Add(ball->pos, Vector2Scale(normal, overlap));

            // Relative velocity
            Vector2 relV = Vector2Subtract(ball->velo, players[i].velo);

            float velAlongNormal = relV.x * normal.x + relV.y * normal.y;
            if (velAlongNormal < 0) {
                float restitution = 0.8f;
                float j = -(1 + restitution) * velAlongNormal;
                // We assume player has infinite mass (doesn't move)
                ball->velo = Vector2Add(ball->velo, Vector2Scale(normal, j));
            }
        }
    }
}
