#include "player.h"
#include "config.h"
#include "macros.h"
#include <math.h>
#include <raymath.h>
#include "game.h"
#include "particle.h"


#define HBRADIUS 60
#define POWERSHOT_BOUNCE 3

#define PART_SIZE 128
static Particle ps[2][PART_SIZE] = {0};

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
    player->powershot = false;

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
    case PLAYER_BLACK:
        player->super.active = true;
        player->super.maxchr_time = 4.0f;
        player->super.maxuse_time = 0.0f;
        break;
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
        player->super.maxchr_time = 7.5f;
        player->super.maxuse_time = 0.0f;
        break;
    default:
        player->super.active = false;
        break;
    }
#if DEBUG
    player->super.maxchr_time = 0.0f;
#endif
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
    draw_particles(ps[player->side], PART_SIZE);
    DrawTexturePro(sheet, src, dest, vec2(0, 0), 0, WHITE);
}

static void desuper(Player *player, Game *game);

static void super(Player *player, Game *game)
{
    if (player->index == PLAYER_MATRIX && player->super.being_used) {
        desuper(player, game); 
    }
    if (!player->super.active || !player->super.charged || player->super.being_used)
        return;

    player->super.charged = false;
    player->super.being_used = true;
    player->super.chr_time = 0;

    Player *opponent = &game->players[
        player->side == PLAYER_SIDE_LEFT
        ? PLAYER_SIDE_RIGHT
        : PLAYER_SIDE_LEFT
    ];
    switch (player->index) {
    case PLAYER_BLACK:
        player->powershot = true;
        break;
    case PLAYER_BALD:
        ascir(player->p).radius = PLAYER_GAME_SIZE * 2.0f;
        player->p.mass *= 6.0f;
        break;
    case PLAYER_MATRIX:
        game->ball.time_scale = 0.25f;
        break;
    case PLAYER_HACKER:
        opponent->revctrl = true;
        emit_particles_rand(
            ps[opponent->side], /* Particle *ps,  */
            PART_SIZE, /* int size,  */
            PARTICLE_SQUARE, /* int type,  */
            PART_SIZE / 2, /* int needed,  */
            ascir(opponent->p).pos, /* Vector2 pos,  */
            vec2zero, /* Vector2 dir,  */
            200, /* float velo,  */
            1.5f, /* float life,  */
            color(0, 200, 0, 255), /* Color c,  */
            ascir(opponent->p).radius /* float psize  */
        );
        break;
    case PLAYER_ALIEN: {
        Vector2 *op_pos_p = &ascir(opponent->p).pos;
        Vector2 op_pos = *op_pos_p;
        *op_pos_p = ascir(player->p).pos;
        ascir(player->p).pos = op_pos;
        static const int needed_part = PART_SIZE / 2;
        emit_particles_rand(
            ps[player->side], /* Particle *ps,  */
            PART_SIZE, /* int size,  */
            PARTICLE_CIRCLE, /* int type,  */
            needed_part, /* int needed,  */
            ascir(player->p).pos, /* Vector2 pos,  */
            vec2zero, /* Vector2 dir,  */
            300, /* float velo,  */
            1.0f, /* float life,  */
            VIOLET, /* Color c,  */
            ascir(player->p).radius /* float psize  */
        );
        emit_particles_rand(
            ps[opponent->side], /* Particle *ps,  */
            PART_SIZE, /* int size,  */
            PARTICLE_CIRCLE, /* int type,  */
            needed_part, /* int needed,  */
            ascir(opponent->p).pos, /* Vector2 pos,  */
            vec2zero, /* Vector2 dir,  */
            300, /* float velo,  */
            1.0f, /* float life,  */
            VIOLET, /* Color c,  */
            ascir(opponent->p).radius /* float psize  */
        );
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

static void hit_straight(Ball *b, float velo, int *powershot, int side)
{
    if (*powershot) {
        velo *= 2;
        *powershot = false;
        b->hitleft_trail = POWERSHOT_BOUNCE; 
    }
    float speed = vec2len(b->p.velo);
    if (speed > velo)
        velo = speed;
    if (side == PLAYER_SIDE_LEFT) {
        b->p.velo.x = velo;
    } else {
        b->p.velo.x = -velo;
    }
    b->p.velo.y = 0;
}

static void hit_diagonal(Ball *b, float velo, int *powershot, int side)
{
    float deg = 37.5f;
    if (*powershot) {
        velo *= 2;
        deg = 22.5f;
        *powershot = false;
        b->hitleft_trail = POWERSHOT_BOUNCE; 
    }
    float deg_in_rad = deg * DEG2RAD;
    float speed = vec2len(b->p.velo);
    if (speed > velo)
        velo = speed;
    b->p.velo.y = -velo * sinf(deg_in_rad);
    if (side == PLAYER_SIDE_LEFT) {
        b->p.velo.x = velo * cosf(deg_in_rad);
    } else {
        b->p.velo.x = -velo * cosf(deg_in_rad);
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
        static const float velo = 1500;
        if (ires.strhit_btn) {
            hit_straight(&game->ball, velo, &player->powershot, player->side);
        }
        if (ires.uphit_btn) {
            hit_diagonal(&game->ball, velo, &player->powershot, player->side);
        }
    }

    player->p.velo.x = player->dir.x * player->speed;
    ascir(player->p).pos.x += player->p.velo.x * dt;

    player->p.velo.y += GRAVITY * dt;
    ascir(player->p).pos.y += player->p.velo.y * dt;

    update_particles(ps[player->side], PART_SIZE, dt);

    if (player->powershot) {
        emit_particles_rand(
            ps[player->side], /* Particle *ps,  */
            PART_SIZE, /* int size,  */
            PARTICLE_CIRCLE, /* int type,  */
            1, /* int needed,  */
            ascir(player->p).pos, /* Vector2 pos,  */
            vec2zero, /* Vector2 dir,  */
            500, /* float velo,  */
            0.5f, /* float life,  */
            color(ORANGE.r - 15, ORANGE.g, ORANGE.b, ORANGE.a), /* Color c,  */
            ascir(player->p).radius /* float psize  */
        );
    }


    /* super */
    if (
        !player->super.active || player->super.charged ||
        player->powershot /* for powershot character */
    ) {
        return;
    }

    if (player->super.being_used) {
        if ((player->super.use_time -= dt) <= 0)
            desuper(player, game);
    } else if ((player->super.chr_time += dt) >= player->super.maxchr_time) {
        player->super.charged = true;
        player->super.chr_time = 0;
        player->super.use_time = player->super.maxuse_time;
    }
}
