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

    /* supers */
    switch (index) {
    case PLAYER_BALD:
        player->super.active = true;
        player->super.charged = false;
        player->super.being_used = false;
        player->super.chr_time = 0.0f;
        player->super.maxchr_time = 5.0f;
        player->super.use_time = 0.0f;
        player->super.maxuse_time = 1.6f;
        break;
    default:
        player->super.active = false;
        break;
    }
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
    Rectangle src  = {
        PLAYER_SPRITE_SIZE * player->index,
        0,
        mul * PLAYER_SPRITE_SIZE,
        PLAYER_SPRITE_SIZE
    };
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

static void super(Player *player)
{
    if (!player->super.active || !player->super.charged || player->super.being_used)
        return;

    player->super.charged = false;
    player->super.being_used = true;
    player->super.chr_time = 0;

    switch (player->index) {
    case PLAYER_BALD:
        ascir(player->p).radius = PLAYER_GAME_SIZE * 2.0f;
        player->p.mass *= 6.0f;
        break;
    }
}

static void desuper(Player *player)
{
    player->super.being_used = false;

    switch (player->index) {
    case PLAYER_BALD:
        ascir(player->p).radius = PLAYER_GAME_SIZE;
        player->p.mass /= 6.0f;
        break;
    }
}

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

        if (IsKeyPressed(KEY_E)) {
            super(player);
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

        if (IsKeyPressed(KEY_RIGHT_CONTROL)) {
            super(player);
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

    /* super */
    if (!player->super.active || player->super.charged)
        return;

    if (player->super.being_used && (player->super.use_time -= dt) <= 0) {
        desuper(player);
    } else if ((player->super.chr_time += dt) >= player->super.maxchr_time) {
        player->super.charged = true;
        player->super.chr_time = 0;
        player->super.use_time = player->super.maxuse_time;
    }
}
