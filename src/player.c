#include "player.h"
#include "config.h"
#include "macros.h"
#include <math.h>
#include <raymath.h>
#include "game.h"


static const float hbradius = 60;

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

    /* ball hitbox */
    player->hboffset =
        side == PLAYER_LEFT_SIDE
        ? vec2(radius / 1.0f, 0)
        : vec2(-radius / 1.0f, 0);

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
    Vector2 hb_pos = Vector2Add(ascir(player->p).pos, player->hboffset);
    DrawCircleV(hb_pos, hbradius, color);
    DrawTexturePro(sheet, src, dest, vec2(0, 0), 0, WHITE);
}

#define gamepadl_exist() (IsGamepadAvailable(0))
#define gamepadl_axis(a) (GetGamepadAxisMovement(0, (a)))
#define gamepadl_btnpressed(b) (IsGamepadButtonPressed(0, (b)))

#define gamepadr_exist() (IsGamepadAvailable(1))
#define gamepadr_axis(a) (GetGamepadAxisMovement(1, (a)))
#define gamepadr_btnpressed(b) (IsGamepadButtonPressed(1, (b)))

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

static void hit_straight(Ball *b, int side)
{
    static const float velox = 1200;
    static const float veloy = -800;
    if (side == PLAYER_LEFT_SIDE) {
        b->p.velo.x = velox;
    } else {
        b->p.velo.x = -velox;
    }
    b->p.velo.y = 0;
}

static void hit_diagonal(Ball *b, int side)
{
    static const float velo = 2000;
    static const float deg = 30 * DEG2RAD;
    b->p.velo.y = -velo * sinf(deg);
    if (side == PLAYER_LEFT_SIDE) {
        b->p.velo.x = velo * cosf(deg);
    } else {
        b->p.velo.x = -velo * cosf(deg);
    }
}

void update_player(Player *player, void *gameptr, float dt)
{
    Game *game = (Game*)gameptr;

    if (player->side == PLAYER_LEFT_SIDE) {
        if (gamepadl_exist()) {
            float ax = gamepadl_axis(GAMEPAD_AXIS_LEFT_X);
            player->dir.x = fabs(ax) > 0.2f ? ax : 0;
            if (player->p.on_ground && gamepadl_axis(GAMEPAD_AXIS_LEFT_Y) < -0.7f) {
                player->p.on_ground = false;
                player->p.velo.y = player->jmp_force;
            }
            if (gamepadl_btnpressed(GAMEPAD_BUTTON_RIGHT_FACE_UP)) {
                super(player);
            }
        } else {
            player->dir.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
            if (player->p.on_ground && IsKeyPressed(KEY_W)) {
                player->p.on_ground = false;
                player->p.velo.y = player->jmp_force;
            }
            if (IsKeyPressed(KEY_E)) {
                super(player);
            }
            Vector2 hb_pos = Vector2Add(ascir(player->p).pos, player->hboffset);
            if (check_cir_coll(ascirp(&game->ball.p), (struct Circle) { hb_pos, hbradius }, NULL, NULL)) {
                if (IsKeyPressed(KEY_F)) {
                    hit_straight(&game->ball, player->side);
                }
                if (IsKeyPressed(KEY_R)) {
                    hit_diagonal(&game->ball, player->side);
                }
            }
        }
    } else if (player->side == PLAYER_RIGHT_SIDE) {
        if (gamepadr_exist()) {
            float ax = gamepadr_axis(GAMEPAD_AXIS_LEFT_X);
            player->dir.x = fabs(ax) > 0.2f ? ax : 0;
            if (player->p.on_ground && gamepadr_axis(GAMEPAD_AXIS_LEFT_Y) < -0.7f) {
                player->p.on_ground = false;
                player->p.velo.y = player->jmp_force;
            }
            if (gamepadr_btnpressed(GAMEPAD_BUTTON_RIGHT_FACE_UP)) {
                super(player);
            }
        } else {
            player->dir.x = IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT);
            if (player->p.on_ground && IsKeyPressed(KEY_UP)) {
                player->p.on_ground = false;
                player->p.velo.y = player->jmp_force;
            }
            if (IsKeyPressed(KEY_RIGHT_CONTROL)) {
                super(player);
            }
            Vector2 hb_pos = Vector2Add(ascir(player->p).pos, player->hboffset);
            if (check_cir_coll(ascirp(&game->ball.p), (struct Circle) { hb_pos, hbradius }, NULL, NULL)) {
                if (IsKeyPressed(KEY_L)) {
                    hit_straight(&game->ball, player->side);
                }
                if (IsKeyPressed(KEY_O)) {
                    hit_diagonal(&game->ball, player->side);
                }
            }
        }
    }

    player->p.velo.x = player->dir.x * player->speed;
    ascir(player->p).pos.x += player->p.velo.x * dt;

    player->p.velo.y += GRAVITY * dt;
    ascir(player->p).pos.y += player->p.velo.y * dt;

    /* super */
    if (!player->super.active || player->super.charged)
        return;

    if (player->super.being_used) {
        if ((player->super.use_time -= dt) <= 0)
            desuper(player);
    } else if ((player->super.chr_time += dt) >= player->super.maxchr_time) {
        player->super.charged = true;
        player->super.chr_time = 0;
        player->super.use_time = player->super.maxuse_time;
    }
}
