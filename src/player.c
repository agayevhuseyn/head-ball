#include "player.h"
#include "config.h"
#include "macros.h"


void init_player(Player *player, int index, int side, Vector2 pos,
                 float radius, float speed, float jmp_force)
{
    *player = (Player) {0};
    /* physics */
    player->p.type = POBJECT_CIR;
    player->p.isstatic = false;
    player->p.bounce = false;
    player->p.fric = 0;
    player->p.mass = 200;
    player->p.eloss = 0;
    ascir(player->p).pos = pos;
    ascir(player->p).radius = radius;

    player->speed = speed;
    player->jmp_force = jmp_force;
    player->side = side;
    player->index = index;
}

void draw_player(Player *player, Texture2D sheet)
{
    Color color;
    int mul;
    if (player->side == PLAYER_LEFT_SIDE) {
        color = RED;
        mul = 1;
    } else {
        color = BLUE;
        mul = -1;
    }

    //DrawCircleV(ascir(player->p).pos, ascir(player->p).radius, color);
    Rectangle src  = { sheet.height * player->index, 0, mul * sheet.height, sheet.height };
    float radius = ascir(player->p).radius;
    Rectangle dest = {
        ascir(player->p).pos.x - radius,
        ascir(player->p).pos.y - radius,
        radius * 2,
        radius * 2
    };
    DrawTexturePro(sheet, src, dest, vec2(0, 0), 0, WHITE);
}

#define gamepad_exist() (IsGamepadAvailable(0))
#define gamepad_axis(a) (GetGamepadAxisMovement(0, (a)))

void update_player(Player *player, float dt)
{
    if (player->side == PLAYER_LEFT_SIDE) {
        if (gamepad_exist()) {
            player->dir.x = gamepad_axis(GAMEPAD_AXIS_LEFT_X);
            if (player->p.on_ground && gamepad_axis(GAMEPAD_AXIS_LEFT_Y) < -0.7f) {
                player->p.on_ground = false;
                player->p.velo.y = player->jmp_force;
            }
        } else {
            player->dir.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
            if (player->p.on_ground && IsKeyPressed(KEY_W)) {
                player->p.on_ground = false;
                player->p.velo.y = player->jmp_force;
            }
        }
    } else if (player->side == PLAYER_RIGHT_SIDE) {
        if (gamepad_exist()) {
            player->dir.x = gamepad_axis(GAMEPAD_AXIS_RIGHT_X);
            if (player->p.on_ground && gamepad_axis(GAMEPAD_AXIS_RIGHT_Y) < -0.7f) {
                player->p.on_ground = false;
                player->p.velo.y = player->jmp_force;
            }
        } else {
            player->dir.x = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT);
            if (player->p.on_ground && IsKeyPressed(KEY_UP)) {
                player->p.on_ground = false;
                player->p.velo.y = player->jmp_force;
            }
        }
    }

    player->p.velo.x = player->dir.x * player->speed;
    ascir(player->p).pos.x += player->p.velo.x * dt;

    player->p.velo.y += GRAVITY * dt;
    ascir(player->p).pos.y += player->p.velo.y * dt;

    if (ascir(player->p).pos.y + ascir(player->p).radius >= GROUND) {
        //ascir(player->p).pos.y = GROUND - ascir(player->p).radius;
        //player->p.velo.y = 0;
        //player->can_jump = true;
    }
}
