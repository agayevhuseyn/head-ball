#include "player.h"
#include "config.h"
#include "emote.h"
#include "macros.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include "game.h"
#include "particle.h"
#include <stdbool.h>
#include <string.h> /* memset */
#include <stdlib.h> /* rand */


#define HBRADIUS 60
#define POWERSHOT_BOUNCE 3
#define DASH_SPEED_MULT 7.0f
#define SUMO_SMASH_MULTIPLIER 1.5f
#define STUN_MAX_TIME 3.0f
#define BALL_SLOWTIME_SCALE 0.25f

#define PART_SIZE 128
static Particle ps[2][PART_SIZE] = {0};

static Shader stunshader;

void init_player(Player *player, int index, int side, Vector2 pos,
                 float radius, float speed, float jmp_force)
{
    if (stunshader.id == 0)
        stunshader = LoadShader(NULL, "shaders/stun.frag");
    /* reset particles */
    memset(ps[side], 0, sizeof(ps[side]));
    /* player */
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
    case PLAYER_BRUNETTE:
        player->super.active = true;
        player->super.maxchr_time = 1.4f;
        player->super.maxuse_time = 0.09f;
        break;
    case PLAYER_BLACK:
        player->super.active = true;
        player->super.maxchr_time = 4.0f;
        player->super.maxuse_time = 0.0f;
        break;
    case PLAYER_BALD:
        player->super.active = true;
        player->super.maxchr_time = 6.0f;
        player->super.maxuse_time = 2.0f;
        break;
    case PLAYER_SUMO:
        player->super.active = true;
        player->super.maxchr_time = 7.5f;
        player->super.maxuse_time = 0.0f;
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
    case PLAYER_JOKER:
        player->super.active = true;
        player->super.maxchr_time = 8.0f;
        player->super.maxuse_time = 4.0f;
        break;
    case PLAYER_ALIEN:
        player->super.active = true;
        player->super.maxchr_time = 9.0f;
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
    float scale_x = 1.0f + player->squash * 1;
    float scale_y = 1.0f - player->squash * 2;
    Vector2 size = vec2(radius * 2.0f * scale_x, radius * 2.0f * scale_y + player->height_diff);
    Rectangle dest = {
        ascir(player->p).pos.x - radius * scale_x,
        ascir(player->p).pos.y - radius * scale_y - player->height_diff,
        size.x,
        size.y
    };
    Vector2 hb_pos = Vector2Add(ascir(player->p).pos, player->hboffset);
    //DrawCircleV(hb_pos, player->hbradius, color);
    float height = ascir(player->p).pos.y - GROUND;
    float shadow_intense = 1.0f - -height / GROUND;
    DrawEllipse(ascir(player->p).pos.x, GROUND + 10, size.x / 2.0f, radius/16, color(0, 0, 0, 120 * shadow_intense));
    draw_particles(ps[player->side], PART_SIZE);
    if (player->stunned) {
        SetShaderValue(stunshader, GetShaderLocation(stunshader, "time"), &player->stun_time, SHADER_UNIFORM_FLOAT);
        BeginShaderMode(stunshader);
    }
    DrawTexturePro(sheet, src, dest, vec2(0, 0), 0, WHITE);
    if (player->stunned) {
        EndShaderMode();
    }

    if (player->stunned) {
        float t = player->stun_time;
        for (int i = 0; i < 3; i++) {
            float a = t * 3.0f + i * (PI*2/3);
            Vector2 pos = {
                ascir(player->p).pos.x + cos(a) * 40.0f,
                ascir(player->p).pos.y - radius + sin(a) * 8.0f
            };
            DrawRectangleV(pos, vec2(16, 16), YELLOW);
        }
    }
}

static void desuper(Player *player, Game *game);

static void super(Player *player, Game *game)
{
    if (player->index == PLAYER_MATRIX && player->super.being_used) {
        desuper(player, game);
        return;
    }
    if (player->index == PLAYER_SUMO && player->p.on_ground) {
        return;
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
    case PLAYER_BRUNETTE:
        player->dash_dir = player->dir.x
            ? player->dir.x
            : (ascir(game->ball.p).pos.x > ascir(player->p).pos.x ? 1 : -1);
        break;
    case PLAYER_BLACK:
        player->powershot = true;
        break;
    case PLAYER_BALD:
        play_egghead(&game->sm);
        player->changing_size = 1;
        player->target_size = PLAYER_GAME_SIZE * 2.0f;
        player->p.mass *= 6.0f;
        break;
    case PLAYER_SUMO:
        player->smashing_ground = true;
        player->p.velo.y = -player->jmp_force;
        break;
    case PLAYER_MATRIX:
        play_slowmo(&game->sm);
        game->ball.time_scale = BALL_SLOWTIME_SCALE;
        break;
    case PLAYER_HACKER:
        opponent->revctrl = true;
        play_hack(&game->sm);
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
    case PLAYER_JOKER:
        play_joker(&game->sm);
        break;
    case PLAYER_ALIEN: {
        Vector2 *op_pos_p = &ascir(opponent->p).pos;
        Vector2 op_pos = *op_pos_p;
        *op_pos_p = ascir(player->p).pos;
        ascir(player->p).pos = op_pos;
        play_alien(&game->sm);
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
        player->changing_size = -1;
        player->target_size = PLAYER_GAME_SIZE;
        player->p.mass /= 6.0f;
        break;
    case PLAYER_MATRIX:
        play_slowmoend(&game->sm);
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
    case PLAYER_JOKER:
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

static Vector2 predict_ballpos(Ball *ball, float dt)
{
    Vector2 pos  = ascir(ball->p).pos;
    Vector2 velo = ball->p.velo;

    velo.y += GRAVITY * dt * ball->time_scale;
    pos.x += velo.x * dt * ball->time_scale;
    pos.y += velo.y * dt * ball->time_scale;

    return pos;
}

static void update_bot(Player *player, PlayerInputResult *ires, Game *game, float dt)
{
    static enum {
        IDLE,
        CHASE,
        ATTACK,
        DEFEND,
        SUPER
    } state = IDLE;

    Ball *ball = &game->ball;
    Vector2 pred_ballpos = predict_ballpos(&game->ball, dt);
    Vector2 player_pos = ascir(player->p).pos;
    if (vec2dist(ascir(player->p).pos, pred_ballpos) < 100) {
        state = IDLE;
    } else if (vec2dist(ascir(player->p).pos, pred_ballpos) < 250) {
        state = ATTACK;
    } else if (ball->p.velo.x > 0) {
        state = DEFEND;
    } else if (0 && ball->p.velo.x > 0) {
    } else {
        state = CHASE;
    }

    switch (state) {
    case IDLE:
        break;
    case CHASE:
        ires->iaxis.x = pred_ballpos.x - player_pos.x < 0 ? -1 : 1;
        break;
    case ATTACK:
        ires->iaxis.x = pred_ballpos.x - player_pos.x < 0 ? -1 : 1;
        if (rand() % 20 == 0) {
            (rand() % 2 == 0)
                ? (ires->strhit_btn = true)
                : (ires->uphit_btn = true);
        }
        break;
    case DEFEND:
        if (rand() % 15 == 0) {
            ires->press_axis.y = -1;
        }
        ires->iaxis.x = 1;
        break;
    case SUPER:
        break;
    }
}

void update_player(Player *player, PlayerInputResult ires, void *gameptr, float dt)
{
    Game *game = (Game*)gameptr;

    if (IsKeyPressed(KEY_GRAVE) && player->side == PLAYER_SIDE_LEFT) {
        send_emote(&game->em, player->side, rand() % 4);
    } else if (IsKeyPressed(KEY_TAB) && player->side == PLAYER_SIDE_RIGHT) {
        send_emote(&game->em, player->side, rand() % 4);
    }

    if (player->is_bot) {
        ires = (PlayerInputResult) {0};
        update_bot(player, &ires, game, dt);
    }
    if (player->stunned) {
        player->stun_time += dt;
        if (player->stun_time >= STUN_MAX_TIME) {
            player->stunned = false;
            player->stun_time = 0;
        } else {
            ires = (PlayerInputResult) {0};
        }
    }

    player->dir.x = player->revctrl ? -ires.iaxis.x : ires.iaxis.x;

    if (player->p.on_ground) {
        if (player->landed) {
            player->landed = false;

            Vector2 pos = ascir(player->p).pos;
            float radius = ascir(player->p).radius;
            emit_particles_rand(
                ps[player->side], /* Particle *ps,  */
                PART_SIZE, /* int size,  */
                PARTICLE_SQUARE, /* int type,  */
                48, /* int needed,  */
                vec2(pos.x, pos.y + radius), /* Vector2 pos,  */
                vec2zero, /* Vector2 dir,  */
                200, /* float velo,  */
                0.4f, /* float life,  */
                color(240, 240, 240, 50), /* Color c,  */
                8.0f /* float psize  */
            );
        }
        if (ires.press_axis.y < 0) {
            player->p.velo.y = player->jmp_force;
            play_jump(&game->sm);
        }
    } else if (player->p.velo.y > 50.0f) {
        player->landed = true;
    }

    if (ires.super_btn)
        super(player, game);

    Vector2 hb_pos = Vector2Add(ascir(player->p).pos, player->hboffset);
    player->hbradius = clamp(HBRADIUS * fabs(game->ball.p.velo.x) / 700, HBRADIUS, 100);
    if (check_cir_coll(ascirp(&game->ball.p), (Circle) { hb_pos, player->hbradius }, NULL, NULL)) {
        static const float velo = 1500;
        int kick = false;
        int powershot = player->powershot;
        if (ires.strhit_btn) {
            kick = true;
            hit_straight(&game->ball, velo, &player->powershot, player->side);
        }
        if (ires.uphit_btn) {
            kick = true;
            hit_diagonal(&game->ball, velo, &player->powershot, player->side);
        }

        if (kick) {
            play_kick(&game->sm);
            if (powershot) {
                game->ball.bomb_time = game->ball.bomb_max_time;
                play_fire(&game->sm);
            }
        }
    }

    player->p.velo.x = player->dir.x * player->speed;
    player->p.velo.y += GRAVITY * dt;

    /* jump squash */
    float target = clamp(player->p.velo.y / (GRAVITY), -0.1f, 0.1f);
    player->squash = lerp(player->squash, target, 8.0f * dt);

    if (player->index == PLAYER_BRUNETTE && player->super.being_used) {
        player->p.velo.x = player->dash_dir * player->speed * DASH_SPEED_MULT;
        player->p.velo.y = 0;
        emit_particles(
            ps[player->side],                 /* Particle *ps,  */
            PART_SIZE,          /* int size,  */
            PARTICLE_CIRCLE,    /* int type,  */
            1,                  /* int needed,  */
            ascir(player->p).pos, /* Vector2 pos,  */
            vec2zero,         /* Vector2 dir,  */
            100,                /* float velo,  */
            0.4f,               /* float life,  */
            WHITE,             /* Color c,  */
            ascir(player->p).radius   /* float psize  */
        );
    }

    ascir(player->p).pos.x += player->p.velo.x * dt;
    ascir(player->p).pos.y += player->p.velo.y * dt;

    /* breath */
    player->breath += dt;
    if (player->breath > PI) {
        player->breath = 0;
    }
    if (player->dir.x != 0 || !player->p.on_ground) {
        if (fabs(player->height_diff) < 1.0f) {
            player->breath = 0;
        }
    }
    player->height_diff = sinf(player->breath * 4) * 8;

    /* EGG */
    static const float change_size_rate = 250.0f;
    if (player->changing_size > 0) {
        if ((ascir(player->p).radius += change_size_rate * dt) >= player->target_size) {
            player->changing_size = 0;
            ascir(player->p).radius = player->target_size;
        }
    } else if (player->changing_size < 0) {
        if ((ascir(player->p).radius -= change_size_rate * dt) <= player->target_size) {
            player->changing_size = 0;
            ascir(player->p).radius = player->target_size;
        }
    }

    /* SUMO */
    if (player->smashing_ground && player->p.on_ground) {
        player->smashing_ground = false;
        Player *opponent = 
            &game->players[
                player->side == PLAYER_SIDE_LEFT
                ? PLAYER_SIDE_RIGHT
                : PLAYER_SIDE_LEFT
            ];

        if (opponent->p.on_ground) {
            opponent->p.velo.y = SUMO_SMASH_MULTIPLIER * opponent->jmp_force;
        }

        emit_particles_rand(
            ps[player->side], /* Particle *ps,  */
            PART_SIZE, /* int size,  */
            PARTICLE_CIRCLE, /* int type,  */
            PART_SIZE / 2, /* int needed,  */
            vec2( /* Vector2 pos,  */
                ascir(player->p).pos.x,
                ascir(player->p).pos.y + ascir(player->p).radius / 2
            ),
            vec2zero, /* Vector2 dir,  */
            600, /* float velo,  */
            0.7f, /* float life,  */
            color(120, 240, 80, 255), /* Color c,  */
            ascir(player->p).radius / 2 /* float psize  */
        );
    }

    /* BLACK */
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

    /* PARTICLES */
    update_particles(ps[player->side], PART_SIZE, dt);

    /* super */
    if (
        !player->super.active || player->super.charged  ||
        player->powershot /* for powershot character */ ||
        player->smashing_ground
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
