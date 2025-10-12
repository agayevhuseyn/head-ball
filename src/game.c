#include "game.h"
#include "macros.h"
#include "config.h"
#include <raylib.h>
#include <raymath.h>


/* GAME */
#define game_onmenu(g)  ((g)->state == GAME_STATE_MENU)
#define game_isrun(g)   ((g)->state == GAME_STATE_RUN)
#define game_onpause(g) ((g)->state == GAME_STATE_PAUSE)

/* BAR */
#define BAR_WIDTH  156
#define BAR_HEIGHT 32
#define BAR_POS_Y  328

#define BOR_THICK 32

/* BALL */
#define BALL_START_POS vec2(WIDTH / 2.0f, 128)

/* PLAYER */
#define JMP_FORCE -950
#define SPEED 400
#define PLAYER_LEFT_START_POS  vec2(256, 400)
#define PLAYER_RIGHT_START_POS vec2(1024, 400)

static Texture2D bg_tex;
static Texture2D player_tex;
static Texture2D ball_tex;
static int left_pick = -1, right_pick = -1;

static void draw_recs(PObject *ps, int size)
{
    for (int i = 0; i < size; i++) {
        DrawRectangleRec(asrec(ps[i]), RED);
    }
}

static void start_game(Game *game, int left, int right)
{
    init_ball(&game->ball, BALL_START_POS, 32);
    init_player(&game->players[0], left, PLAYER_LEFT_SIDE,  PLAYER_LEFT_START_POS,  64, SPEED, JMP_FORCE);
    init_player(&game->players[1], right, PLAYER_RIGHT_SIDE, PLAYER_RIGHT_START_POS, 64, SPEED, JMP_FORCE);
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

static void draw_ui(Game *game)
{
    Vector2 size = vec2(256, 256);
    Vector2 pos  = vec2(80, (HEIGHT - size.y) / 2);

    int left_i  = IsKeyPressed(KEY_D) - IsKeyPressed(KEY_A);
    int right_i = IsKeyPressed(KEY_RIGHT) - IsKeyPressed(KEY_LEFT);

    if (left_i > 0) {
        if ((left_pick + left_i) % PLAYER_SIZE == right_pick)
            left_i++;

        left_pick = (left_pick + left_i) % PLAYER_SIZE;
    } else if (left_i < 0) {
        if ((left_pick + left_i + PLAYER_SIZE) % PLAYER_SIZE == right_pick)
            left_i--;

        left_pick = (left_pick + left_i + PLAYER_SIZE) % PLAYER_SIZE;
    }

    if (right_i > 0) {
        if ((right_pick + right_i) % PLAYER_SIZE == left_pick)
            right_i++;

        right_pick = (right_pick + right_i) % PLAYER_SIZE;
    } else if (right_i < 0) {
        if ((right_pick + right_i + PLAYER_SIZE) % PLAYER_SIZE == left_pick)
            right_i--;

        right_pick = (right_pick + right_i + PLAYER_SIZE) % PLAYER_SIZE;
    }

    for (int i = 0; i < PLAYER_SIZE; i++) {
        Rectangle src  = { player_tex.height * i, 0, player_tex.height, player_tex.height };
        Rectangle dest = {
            pos.x,
            pos.y,
            size.x,
            size.y
        };
        Rectangle frame = rec(pos.x - 8, pos.y - 8, size.x + 16, size.y + 16);
        Color c = WHITE;
        if (left_pick == i)
            c = RED;
        else if (right_pick == i)
            c = BLUE;

        DrawRectangleRounded(frame, 0.15f, 0, c);
        DrawTexturePro(player_tex, src, dest, vec2(0, 0), 0, WHITE);
        pos.x += size.x + 32;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (left_pick != -1 && right_pick != -1) {
            game->state = GAME_STATE_RUN;
            start_game(game, left_pick, right_pick);
        }
    }
}

void init_game(Game *game)
{
    /* game */
    game->state = GAME_STATE_MENU;
    /* textures */
    bg_tex = LoadTexture("assets/bg.png");
    player_tex = LoadTexture("assets/player.png");
    ball_tex = LoadTexture("assets/ball.png");
    /* ball */
    /* players */
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
    if (game_onmenu(game)) {
        draw_ui(game);
        return;
    }
    //draw_recs(game, 2);
    draw_player(&game->players[0], player_tex);
    draw_player(&game->players[1], player_tex);
    draw_ball(&game->ball, ball_tex);
    //draw_recs(game->borders, 4);
    if (IsKeyPressed(KEY_F1))
        show_fps = !show_fps;
    if (show_fps)
        DrawText(TextFormat("%i", GetFPS()), 8, 8, 32, BLACK);
}

void update_game(Game *game, float dt)
{
    if (game_onmenu(game)) {
        return;
    }
    if (IsKeyPressed(KEY_P)) {
        game->state = game_onpause(game) ? GAME_STATE_RUN : GAME_STATE_PAUSE;
    }
    if (game_onpause(game)) {
        return;
    }
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
