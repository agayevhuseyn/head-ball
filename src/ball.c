#include "ball.h"
#include "config.h"
#include <math.h>
#include "macros.h"

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
  ball->velo.y += GRAVITY * dt;
  ball->pos.y  += ball->velo.y * dt;
  ball->pos.x  += ball->velo.x * dt;

  if (ball->pos.y + ball->radius >= HEIGHT) {
    ball->pos.y = HEIGHT - ball->radius;
    ball->velo.y *= -0.8;
  }
  if (ball->pos.x - ball->radius <= 0) {
    ball->pos.x = ball->radius;
    ball->velo.x *= -1;
  } else if (ball->pos.x + ball->radius >= WIDTH) {
    ball->pos.x = WIDTH - ball->radius;
    ball->velo.x *= -1;
  }

  for (int i = 0; i < 2; i++) {
  Vector2 distance = {
       players[i].pos.x - ball->pos.x,
       players[i].pos.y - ball->pos.y
    };
    
    float distanceLength = sqrtf(distance.x*distance.x + distance.y*distance.y);
    float minDistance =ball->radius + players[i].radius;
    
    // Check if circles are colliding
    if (distanceLength < minDistance) {
        // Calculate collision normal (unit vector)
        Vector2 normal = {
            distance.x / distanceLength,
            distance.y / distanceLength
        };
        
        // Calculate relative velocity
        Vector2 relativeVelocity = {
           players[i].speed * players[i].dir.x - ball->velo.x,
           players[i].speed - ball->velo.y
        };
        
        // Calculate impulse along normal
        float impulse = (relativeVelocity.x * normal.x + relativeVelocity.y * normal.y);
        
        // Only collide if objects are moving towards each other
        if (impulse < 0) {
            // Calculate restitution (bounciness)
            float restitution = 0.8f; // Adjust for desired bounciness
            
            // Apply impulse to velocities
            float j = -(1 + restitution) * impulse;
            
            // Apply to circle a (ball)
           ball->velo.x -= j * normal.x;
           ball->velo.y -= j * normal.y;
            
            // Apply to circle b (player)
            
            // Separate circles to prevent sticking
            float overlap = minDistance - distanceLength;
            
           ball->pos.x -= overlap * normal.x;
           ball->pos.y -= overlap * normal.y;
        }
    }
  }





  return;

  for (int i = 0; i < 2; i++) {
    Vector2 col = check_circle_collision(ball->pos, players[i].pos, ball->radius, players[i].radius);
    if (col.x != 0 || col.y != 0) {
      ball->pos = vec2(
        players[i].pos.x - col.x * (ball->radius + players[i].radius + 10),
        players[i].pos.y - col.y * (ball->radius + players[i].radius + 10)
      );

      // Reflect velocity
      float dot = ball->velo.x * col.x + ball->velo.y * col.y;
      ball->velo.x -= 2 * dot * col.x;
      ball->velo.y -= 2 * dot * col.y;
      //ball->velo.y *= -1;

      //ball->velo.x += players[i].speed * 0.2f;
      //ball->velo.y += players[i].speed * 0.2f;
    }
  }
}
