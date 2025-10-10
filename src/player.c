#include "player.h"
#include "config.h"

void init_player(Player *player, int side, Vector2 pos, Vector2 dir,
                 float radius, float speed, float jmp_force)
{
    *player = (Player) {0};
    player->pos = pos;
    player->dir = dir;
    player->radius = radius;
    player->speed = speed;
    player->jmp_force = jmp_force;
    player->can_jump = false;
    player->side = side;
}

void draw_player(Player *player)
{
    Color color;
    if (player->side == PLAYER_LEFT_SIDE)
        color = RED;
    else
        color = BLUE;

    DrawCircleV(player->pos, player->radius, color);
}

#define gamepad_exist() (IsGamepadAvailable(0))
#define gamepad_axis(a) (GetGamepadAxisMovement(0, (a)))

void update_player(Player *player, float dt)
{
    if (player->side == PLAYER_LEFT_SIDE) {
        if (gamepad_exist()) {
            player->dir.x = gamepad_axis(GAMEPAD_AXIS_LEFT_X);
            if (player->can_jump && gamepad_axis(GAMEPAD_AXIS_LEFT_Y) < -0.7f) {
                player->can_jump = false;
                player->velo.y = player->jmp_force;
            }
        } else {
            player->dir.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
            if (player->can_jump && IsKeyPressed(KEY_W)) {
                player->can_jump = false;
                player->velo.y = player->jmp_force;
            }
        }
    } else if (player->side == PLAYER_RIGHT_SIDE) {
        if (gamepad_exist()) {
            player->dir.x = gamepad_axis(GAMEPAD_AXIS_RIGHT_X);
            if (player->can_jump && gamepad_axis(GAMEPAD_AXIS_RIGHT_Y) < -0.7f) {
                player->can_jump = false;
                player->velo.y = player->jmp_force;
            }
        } else {
            player->dir.x = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT);
            if (player->can_jump && IsKeyPressed(KEY_UP)) {
                player->can_jump = false;
                player->velo.y = player->jmp_force;
            }
        }
    }

    player->velo.x = player->dir.x * player->speed;
    player->pos.x += player->velo.x * dt;

    player->velo.y  += GRAVITY * dt;
    player->pos.y += player->velo.y * dt;

    if (player->pos.y + player->radius >= HEIGHT) {
        player->pos.y = HEIGHT - player->radius;
        player->velo.y = 0;
        player->can_jump = true;
    }

    if (player->pos.x - player->radius < 0) {
        player->pos.x = player->radius;
    } else if (player->pos.x + player->radius > WIDTH) {
        player->pos.x = WIDTH - player->radius;
    }
}
