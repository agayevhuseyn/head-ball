#include "player.h"
#include "config.h"
#include "macros.h"
#include <math.h>
#include <raymath.h>
#include "game.h"


#define HBRADIUS 60

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
    player->revctrl = false;

    /* ball hitbox */
    player->hboffset =
        side == PLAYER_SIDE_LEFT
        ? vec2(radius / 1.0f, 0)
        : vec2(-radius / 1.0f, 0);

    /* supers */
    player->super.charged = false;
    player->super.being_used = false;
    player->super.chr_time = 0.0f;
    player->super.use_time = 0.0f;
    switch (index) {
    case PLAYER_BALD:
        player->super.active = true;
        player->super.maxchr_time = 5.0f;
        player->super.maxuse_time = 1.6f;
        break;
    case PLAYER_MATRIX:
        player->super.active = true;
        player->super.maxchr_time = 10.0f;
        player->super.maxuse_time = 5.0f;
        break;
    case PLAYER_HACKER:
        player->super.active = true;
        player->super.maxchr_time = 8.0f;
        player->super.maxuse_time = 3.0f;
        break;
    case PLAYER_ALIEN:
        player->super.active = true;
        player->super.maxchr_time = 1;
        player->super.maxuse_time = 0.0f;
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
    if (player->side == PLAYER_SIDE_LEFT) {
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
    //DrawCircleV(hb_pos, player->hbradius, color);
    DrawTexturePro(sheet, src, dest, vec2(0, 0), 0, WHITE);
}

#define gamepadl_exist() (IsGamepadAvailable(0))
#define gamepadl_axis(a) (GetGamepadAxisMovement(0, (a)))
#define gamepadl_btnpressed(b) (IsGamepadButtonPressed(0, (b)))

#define gamepadr_exist() (IsGamepadAvailable(1))
#define gamepadr_axis(a) (GetGamepadAxisMovement(1, (a)))
#define gamepadr_btnpressed(b) (IsGamepadButtonPressed(1, (b)))

static void super(Player *player, Game *game)
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
    case PLAYER_MATRIX:
        game->ball.time_scale = 0.25f;
        break;
    case PLAYER_HACKER:
        game->players[
            player->side == PLAYER_SIDE_LEFT
            ? PLAYER_SIDE_RIGHT
            : PLAYER_SIDE_LEFT
        ]
        .revctrl = true;
        break;
    case PLAYER_ALIEN: {
        Vector2 *op_pos_p = 
            &ascir(game->players[
                player->side == PLAYER_SIDE_LEFT
                ? PLAYER_SIDE_RIGHT
                : PLAYER_SIDE_LEFT
            ].p)
            .pos;
        Vector2 op_pos = *op_pos_p;
        *op_pos_p = ascir(player->p).pos;
        ascir(player->p).pos = op_pos;
        break;
    }
    }
}

static void desuper(Player *player, Game *game)
{
    player->super.being_used = false;

    switch (player->index) {
    case PLAYER_BALD:
        ascir(player->p).radius = PLAYER_GAME_SIZE;
        player->p.mass /= 6.0f;
        break;
    case PLAYER_MATRIX:
        game->ball.time_scale = 1.0f;
        break;
    case PLAYER_HACKER:
        game->players[
            player->side == PLAYER_SIDE_LEFT
            ? PLAYER_SIDE_RIGHT
            : PLAYER_SIDE_LEFT
        ]
        .revctrl = false;
        break;
    }
}

static void hit_straight(Ball *b, int side)
{
    static const float velox = 2000;
    if (side == PLAYER_SIDE_LEFT) {
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
    if (side == PLAYER_SIDE_LEFT) {
        b->p.velo.x = velo * cosf(deg);
    } else {
        b->p.velo.x = -velo * cosf(deg);
    }
}

void update_player(Player *player, void *gameptr, float dt)
{
    Game *game = (Game*)gameptr;
    PlayerInputResult ires = get_playerinputresult(&game->controls[player->side]);

    player->dir.x = player->revctrl ? -ires.iaxis.x : ires.iaxis.x;

    if (ires.press_axis.y < 0 && player->p.on_ground) {
        player->p.on_ground = false;
        player->p.velo.y = player->jmp_force;
    }

    if (ires.super_btn)
        super(player, game);

    Vector2 hb_pos = Vector2Add(ascir(player->p).pos, player->hboffset);
    player->hbradius = clamp(HBRADIUS * fabs(game->ball.p.velo.x) / 700, HBRADIUS, 100);
    if (check_cir_coll(ascirp(&game->ball.p), (Circle) { hb_pos, player->hbradius }, NULL, NULL)) {
        if (ires.strhit_btn) {
            hit_straight(&game->ball, player->side);
        }
        if (ires.uphit_btn) {
            hit_diagonal(&game->ball, player->side);
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
            desuper(player, game);
    } else if ((player->super.chr_time += dt) >= player->super.maxchr_time) {
        player->super.charged = true;
        player->super.chr_time = 0;
        player->super.use_time = player->super.maxuse_time;
    }
}
