#include "game.h"
#include "macros.h"
#include "config.h"
#include <raymath.h>

#define JMP_FORCE -950
#define SPEED 400

/* BAR */
#define BAR_WIDTH  156
#define BAR_HEIGHT 32
#define BAR_POS_Y  328

#define BOR_THICK 32

/* BALL */
#define BALL_START_POS vec2(WIDTH / 2.0f, 128)

/* PLAYER */
#define PLAYER_LEFT_START_POS  vec2(256, 400)
#define PLAYER_RIGHT_START_POS vec2(1024, 400)

static Texture2D bg_tex;

static void draw_recs(PObject *ps, int size)
{
    for (int i = 0; i < size; i++) {
        DrawRectangleRec(asrec(ps[i]), RED);
    }
}

static void reset_game(Game *game)
{
    Player *a  = &game->players[0];
    Player *b  = &game->players[1];
    Ball *ball = &game->ball;

    ascir(ball->p).pos = BALL_START_POS;
    ball->p.velo = vec2zero;

    ascir(a->p).pos = PLAYER_LEFT_START_POS;
    a->p.velo = vec2zero;
    ascir(b->p).pos = PLAYER_RIGHT_START_POS;
    b->p.velo = vec2zero;
}

void init_game(Game *game)
{
    bg_tex = LoadTexture("assets/bg.png");
    /* ball */
    init_ball(&game->ball, BALL_START_POS, 32);
    /* players */
    init_player(&game->players[0], PLAYER_BALD, PLAYER_LEFT_SIDE,  PLAYER_LEFT_START_POS,  64, SPEED, JMP_FORCE);
    init_player(&game->players[1], PLAYER_BLACK, PLAYER_RIGHT_SIDE, PLAYER_RIGHT_START_POS, 64, SPEED, JMP_FORCE);
    /* bars */
    game->bars[0] = pobject_staticrec(rec(0, BAR_POS_Y, BAR_WIDTH, BAR_HEIGHT));
    game->bars[1] = pobject_staticrec(rec(WIDTH - BAR_WIDTH, BAR_POS_Y, BAR_WIDTH, BAR_HEIGHT));
    /* borders */
    game->borders[0] = pobject_staticrec(rec(0, 0 - BOR_THICK, WIDTH, BOR_THICK));
    game->borders[1] = pobject_staticrec(rec(0, GROUND, WIDTH, BOR_THICK));
    game->borders[2] = pobject_staticrec(rec(0 - BOR_THICK, 0, BOR_THICK, GROUND));
    game->borders[3] = pobject_staticrec(rec(WIDTH, 0, BOR_THICK, GROUND));
}

static int show_fps = false;

void draw_game(Game *game)
{
    DrawTextureEx(bg_tex, vec2(0, 0), 0, SCALE, WHITE);
    //draw_recs(game, 2);
    draw_player(&game->players[0]);
    draw_player(&game->players[1]);
    draw_ball(&game->ball);
    //draw_recs(game->borders, 4);
    if (IsKeyPressed(KEY_F1))
        show_fps = !show_fps;
    if (show_fps)
        DrawText(TextFormat("%i", GetFPS()), 8, 8, 32, BLACK);
}

void update_game(Game *game, float dt)
{
    Player *a  = &game->players[0];
    Player *b  = &game->players[1];
    Ball *ball = &game->ball;

    if (IsKeyPressed(KEY_R)) {
        reset_game(game);
    }

    update_player(a, dt);
    update_player(b, dt);

    update_ball(&game->ball, dt);

    handle_coll(&a->p, &b->p, dt);
    handle_coll(&ball->p, &a->p, dt);
    handle_coll(&ball->p, &b->p, dt);

    a->p.on_ground = b->p.on_ground = 0;
    for (int i = 0; i < 4; i++) {
        handle_coll(&ball->p, &game->borders[i], dt);
        handle_coll(&a->p, &game->borders[i], dt);
        handle_coll(&b->p, &game->borders[i], dt);
    }
    for (int i = 0; i < 2; i++) {
        handle_coll(&ball->p, &game->bars[i], dt);
        handle_coll(&a->p, &game->bars[i], dt);
        handle_coll(&b->p, &game->bars[i], dt);
    }
}
