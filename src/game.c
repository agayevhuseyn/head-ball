#include "game.h"
#include "macros.h"
#include "config.h"
#include "player.h"
#include <raylib.h>
#include <raymath.h>
#include "font.h"
#include <time.h>
#include <stdlib.h>
#include "ui.h"
#include <stdio.h>


/* STATE */
#define game_onmenu(g)  ((g)->game_state == GAME_STATE_MENU)
#define game_isrun(g)   ((g)->game_state == GAME_STATE_RUN)
#define game_onpause(g) ((g)->game_state == GAME_STATE_PAUSE)

#define menu_ismain(g)  ((g)->menu_state == MENU_STATE_MAIN)
#define menu_ismode(g)  ((g)->menu_state == MENU_STATE_PLAYMODE)
#define menu_isballpick(g)  ((g)->menu_state == MENU_STATE_BALLPICK)
#define menu_ischarpick(g)  ((g)->menu_state == MENU_STATE_CHARPICK)
#define menu_issettings(g)  ((g)->menu_state == MENU_STATE_SETTINGS)


#define reset_ballpick(g) ((g)->ball_pick = -1)
#define reset_charpicks(g) ( \
    (g)->char_picks[PLAYER_SIDE_LEFT] = (g)->char_picks[PLAYER_SIDE_RIGHT] = -1 \
)
#define reset_picks(g) (reset_ballpick(g), reset_charpicks(g))

#define mode_issingle(g) ((g)->game_mode == GAME_MODE_SINGLE)
#define mode_ismulti(g)  ((g)->game_mode == GAME_MODE_MULTI)

/* BAR */
#define BAR_WIDTH  132
#define BAR_HEIGHT 24
#define BAR_POS_Y  292

#define BOR_THICK 64

/* BALL */
#define BALL_START_POS vec2(WIDTH / 2.0f, 128)

/* PLAYER */
#define JMP_FORCE -950
#define SPEED 400
#define PLAYER_LEFT_START_POS  vec2(256, 400)
#define PLAYER_RIGHT_START_POS vec2(1024, 400)

/* CAMERA */
#define CAM_BASE_OFFSET vec2(WIDTH / 2.0f, HEIGHT / 2.0f)

/* CONTROLS */
static PlayerControl control_left = { /* left */
    .gamepad_id = PLAYER_SIDE_LEFT,
    .gamepad = {
        .super_btn  = GAMEPAD_BUTTON_RIGHT_TRIGGER_2,
        .strhit_btn = GAMEPAD_BUTTON_RIGHT_FACE_DOWN,
        .uphit_btn  = GAMEPAD_BUTTON_RIGHT_FACE_LEFT,
        .forw_btn   = GAMEPAD_BUTTON_RIGHT_FACE_DOWN,
        .back_btn   = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,
    },
    .keyboard = {
        .left_btn   = KEY_A,
        .right_btn  = KEY_D,
        .up_btn     = KEY_W,
        .down_btn   = KEY_S,
        .super_btn  = KEY_SPACE,
        .strhit_btn = KEY_F,
        .uphit_btn  = KEY_E,
    }
};

static PlayerControl control_right = { /* right */
    .gamepad_id = PLAYER_SIDE_RIGHT,
    .gamepad = {
        .super_btn  = GAMEPAD_BUTTON_RIGHT_TRIGGER_2,
        .strhit_btn = GAMEPAD_BUTTON_RIGHT_FACE_DOWN,
        .uphit_btn  = GAMEPAD_BUTTON_RIGHT_FACE_LEFT,
        .forw_btn   = GAMEPAD_BUTTON_RIGHT_FACE_DOWN,
        .back_btn   = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,
    },
    .keyboard = {
        .left_btn   = KEY_LEFT,
        .right_btn  = KEY_RIGHT,
        .up_btn     = KEY_UP,
        .down_btn   = KEY_DOWN,
        .super_btn  = KEY_ENTER,
        .strhit_btn = KEY_RIGHT_CONTROL,
        .uphit_btn  = KEY_RIGHT_SHIFT,
    }
};

static RenderTexture2D rtex;
static Texture2D player_tex;
static Texture2D ball_tex;
static int left_score = 0, right_score = 0;
static int show_fps = false;
static PlayerInputResult lres, rres;

typedef struct {
    const char *name;
    const char *info;
    const char *super;
} CharInfoBox;

static CharInfoBox char_info_boxes[PLAYER_SIZE] = {
    [PLAYER_BRUNETTE] = {
    },
    [PLAYER_BLACK] = {
    },
    [PLAYER_BALD] = {
    },
    [PLAYER_SUMO] = {
    },
    [PLAYER_MATRIX] = {
    },
    [PLAYER_HACKER] = {
    },
    [PLAYER_PANDA] = {
    },
    [PLAYER_ALIEN] = {
    },
};

#define BUTTON_SIZE_WIDE vec2(256, 64)
#define BUTTON_GAP 96
enum {
    BUTTON_MAIN_PLAY,
    BUTTON_MAIN_SETTINGS,
    BUTTON_MAIN_SIZE
};
static Button main_buttons[BUTTON_MAIN_SIZE];

static void init_main_buttons()
{
    main_buttons[BUTTON_MAIN_PLAY] = (Button) {
        .rec = rec(
            (WIDTH - BUTTON_SIZE_WIDE.x) / 2.0f,
            (HEIGHT / 2.0f) - BUTTON_GAP * 2.0f,
            BUTTON_SIZE_WIDE.x,
            BUTTON_SIZE_WIDE.y
        ),
        .text = "Play"
    };
    main_buttons[BUTTON_MAIN_SETTINGS] = (Button) {
        .rec = rec(
            (WIDTH - BUTTON_SIZE_WIDE.x) / 2.0f,
            (HEIGHT / 2.0f) - BUTTON_GAP * 1.0f,
            BUTTON_SIZE_WIDE.x,
            BUTTON_SIZE_WIDE.y
        ),
        .text = "Settings"
    };
}

static void draw_main_buttons(Game *game, PlayerInputResult *lres, PlayerInputResult *rres)
{
    static int selected_button = -1;
    static int prev_selected_button = -1;

    int move_y = lres->press_axis.y + rres->press_axis.y;
    int clicked = lres->forw_btn || rres->forw_btn;

    if (move_y) {
        prev_selected_button = selected_button;
        selected_button += move_y;

        if (selected_button < 0)
            selected_button = BUTTON_MAIN_SIZE - 1;
        else if (selected_button >= BUTTON_MAIN_SIZE)
            selected_button = 0;

        main_buttons[prev_selected_button].hovered = false;
        main_buttons[selected_button].hovered = true;
    }

    if (clicked) {
        switch (selected_button) {
        case BUTTON_MAIN_PLAY:
            game->menu_state = MENU_STATE_PLAYMODE;
            break;
        case BUTTON_MAIN_SETTINGS:
            game->menu_state = MENU_STATE_SETTINGS;
            break;
        }
    }

    for (int i = 0; i < BUTTON_MAIN_SIZE; i++) {
        draw_button(&main_buttons[i]);
    }
}

enum {
    BUTTON_SETTINGS_LP_KEY_LEFT,
    BUTTON_SETTINGS_LP_KEY_RIGHT,
    BUTTON_SETTINGS_LP_KEY_UP,
    BUTTON_SETTINGS_LP_KEY_DOWN,
    BUTTON_SETTINGS_LP_KEY_SUPER,
    BUTTON_SETTINGS_LP_KEY_STRHIT,
    BUTTON_SETTINGS_LP_KEY_UPHIT,
    BUTTON_SETTINGS_RP_KEY_LEFT,
    BUTTON_SETTINGS_RP_KEY_RIGHT,
    BUTTON_SETTINGS_RP_KEY_UP,
    BUTTON_SETTINGS_RP_KEY_DOWN,
    BUTTON_SETTINGS_RP_KEY_SUPER,
    BUTTON_SETTINGS_RP_KEY_STRHIT,
    BUTTON_SETTINGS_RP_KEY_UPHIT,
    BUTTON_SETTINGS_SIZE
};
static Button settings_buttons[BUTTON_SETTINGS_SIZE];

static void init_settings_buttons()
{
    Vector2 start_pos = vec2(WIDTH / 2.0 - BUTTON_SIZE_WIDE.x - BUTTON_GAP, 32);
    Vector2 pos = start_pos;
    for (int i = 0; i < BUTTON_SETTINGS_SIZE; i++) {
        settings_buttons[i] = (Button) {
            .rec = recv(
                pos, BUTTON_SIZE_WIDE
            )
        };
        const char *text;
        switch (i % (BUTTON_SETTINGS_SIZE / 2)) {
        case 0: text = "Left"; break;
        case 1: text = "Right"; break;
        case 2: text = "Up"; break;
        case 3: text = "Down"; break;
        case 4: text = "Super"; break;
        case 5: text = "Hit"; break;
        case 6: text = "Chip"; break;
        }
        settings_buttons[i].text = text;

        if (i + 1 == BUTTON_SETTINGS_SIZE / 2) {
            pos.x = WIDTH / 2.0f + BUTTON_GAP;
            pos.y = start_pos.y;
        } else {
            pos.y += BUTTON_SIZE_WIDE.y + 16;
        }
    }
}

static void draw_settings_buttons(Game *game, PlayerInputResult *lres, PlayerInputResult *rres)
{
    static int selected_button = -1;
    static int prev_selected_button = -1;
    static int col = 0, row = 0;
    static const int btn_each_col = BUTTON_SETTINGS_SIZE / 2;
    static int waiting_for_input = false;

    int move_y = lres->press_axis.y + rres->press_axis.y;
    int move_x = lres->press_axis.x + rres->press_axis.x;
    int clicked = !waiting_for_input && (lres->forw_btn || rres->forw_btn);

    if (!waiting_for_input && (move_x || move_y)) {
        if (col + move_x < 0)
            col = 1;
        else if (col + move_x > 1)
            col = 0;
        else
            col += move_x;

        if (row + move_y < 0)
            row = btn_each_col - 1;
        else if (row + move_y >= btn_each_col)
            row = 0;
        else
            row += move_y;

        prev_selected_button = selected_button;
        selected_button = col * btn_each_col + row;

        settings_buttons[prev_selected_button].hovered = false;
        settings_buttons[selected_button].hovered = true;
    }

    if (waiting_for_input) {
        PlayerControl *pc =
            &game->controls[
                selected_button < btn_each_col
                    ? PLAYER_SIDE_LEFT
                    : PLAYER_SIDE_RIGHT
            ];
        int pressed_key = GetKeyPressed();
        if (pressed_key) {
            waiting_for_input = false;
            switch (selected_button) {
            case BUTTON_SETTINGS_LP_KEY_LEFT: case BUTTON_SETTINGS_RP_KEY_LEFT:
                pc->keyboard.left_btn = pressed_key;
                break;
            case BUTTON_SETTINGS_LP_KEY_RIGHT: case BUTTON_SETTINGS_RP_KEY_RIGHT:
                pc->keyboard.right_btn = pressed_key;
                break;
            case BUTTON_SETTINGS_LP_KEY_UP: case BUTTON_SETTINGS_RP_KEY_UP:
                pc->keyboard.up_btn = pressed_key;
                break;
            case BUTTON_SETTINGS_LP_KEY_DOWN: case BUTTON_SETTINGS_RP_KEY_DOWN:
                pc->keyboard.down_btn = pressed_key;
                break;
            case BUTTON_SETTINGS_LP_KEY_SUPER: case BUTTON_SETTINGS_RP_KEY_SUPER:
                pc->keyboard.super_btn = pressed_key;
                break;
            case BUTTON_SETTINGS_LP_KEY_STRHIT: case BUTTON_SETTINGS_RP_KEY_STRHIT:
                pc->keyboard.strhit_btn = pressed_key;
                break;
            case BUTTON_SETTINGS_LP_KEY_UPHIT: case BUTTON_SETTINGS_RP_KEY_UPHIT:
                pc->keyboard.uphit_btn = pressed_key;
                break;
            }
        }
    }

    if (clicked) {
        waiting_for_input = true; 
    }

    for (int i = 0; i < BUTTON_SETTINGS_SIZE; i++) {
        Button b = settings_buttons[i];
        if (i == selected_button && waiting_for_input)
            b.pressed = true;

        int key;
        PlayerControl *lc = &game->controls[PLAYER_SIDE_LEFT];
        PlayerControl *rc = &game->controls[PLAYER_SIDE_RIGHT];
        switch (i) {
        case BUTTON_SETTINGS_LP_KEY_LEFT:
            key = lc->keyboard.left_btn;
            break;
        case BUTTON_SETTINGS_RP_KEY_LEFT:
            key = rc->keyboard.left_btn;
            break;
        case BUTTON_SETTINGS_LP_KEY_RIGHT:
            key = lc->keyboard.right_btn;
            break;
        case BUTTON_SETTINGS_RP_KEY_RIGHT:
            key = rc->keyboard.right_btn;
            break;
        case BUTTON_SETTINGS_LP_KEY_UP:
            key = lc->keyboard.up_btn;
            break;
        case BUTTON_SETTINGS_RP_KEY_UP:
            key = rc->keyboard.up_btn;
            break;
        case BUTTON_SETTINGS_LP_KEY_DOWN:
            key = lc->keyboard.down_btn;
            break;
        case BUTTON_SETTINGS_RP_KEY_DOWN:
            key = rc->keyboard.down_btn;
            break;
        case BUTTON_SETTINGS_LP_KEY_SUPER:
            key = lc->keyboard.super_btn;
            break;
        case BUTTON_SETTINGS_RP_KEY_SUPER:
            key = rc->keyboard.super_btn;
            break;
        case BUTTON_SETTINGS_LP_KEY_STRHIT:
            key = lc->keyboard.strhit_btn;
            break;
        case BUTTON_SETTINGS_RP_KEY_STRHIT:
            key = rc->keyboard.strhit_btn;
            break;
        case BUTTON_SETTINGS_LP_KEY_UPHIT:
            key = lc->keyboard.uphit_btn;
            break;
        case BUTTON_SETTINGS_RP_KEY_UPHIT:
            key = rc->keyboard.uphit_btn;
            break;
        }
        b.text = TextFormat("%s: %d", settings_buttons[i].text, key);
        draw_button(&b);
    }
}

static int save_settings(Game *game)
{
    FILE *f = fopen("settings.conf", "wb");
    if (!f)
        return 1;

    fwrite(game->controls, sizeof(game->controls[0]), 2, f);
    fclose(f);
    return 0;
}

static int load_settings(Game *game)
{
    FILE *f = fopen("settings.conf", "rb");
    if (!f)
        return 1;

    fread(game->controls, sizeof(game->controls[0]), 2, f);
    fclose(f);
    return 0;
}

static Button mode_buttons[GAME_MODE_SIZE];

static void init_mode_buttons()
{
    mode_buttons[GAME_MODE_SINGLE] = (Button) {
        .rec = rec(
            WIDTH / 2.0f - BUTTON_SIZE_WIDE.x - BUTTON_GAP / 2.0f,
            HEIGHT / 2.0f - BUTTON_SIZE_WIDE.x / 1.3f,
            BUTTON_SIZE_WIDE.x,
            BUTTON_SIZE_WIDE.x
        ),
        .text = "1P"
    };
    mode_buttons[GAME_MODE_MULTI] = (Button) {
        .rec = rec(
            WIDTH / 2.0f + BUTTON_GAP / 2.0f,
            HEIGHT / 2.0f - BUTTON_SIZE_WIDE.x / 1.3f,
            BUTTON_SIZE_WIDE.x,
            BUTTON_SIZE_WIDE.x
        ),
        .text = "2P"
    };
}

static void draw_mode_buttons(Game *game, PlayerInputResult *lres, PlayerInputResult *rres)
{
    static int selected_button = -1;
    static int prev_selected_button = -1;

    int move_x = lres->press_axis.x + rres->press_axis.x;
    int clicked = lres->forw_btn || rres->forw_btn;

    if (move_x) {
        prev_selected_button = selected_button;
        selected_button += move_x;

        if (selected_button < 0)
            selected_button = 1;
        else if (selected_button >= 2)
            selected_button = 0;

        mode_buttons[prev_selected_button].hovered = false;
        mode_buttons[selected_button].hovered = true;
    }

    if (clicked && selected_button != -1) {
        switch (selected_button) {
        case GAME_MODE_SINGLE:
            puts("Singleplayer mode selected");
            game->game_mode = GAME_MODE_SINGLE;
            break;
        case GAME_MODE_MULTI:
            puts("Multiplayer mode selected");
            game->game_mode = GAME_MODE_MULTI;
            break;
        }
        reset_ballpick(game);
        game->menu_state = MENU_STATE_BALLPICK;
    }

    for (int i = 0; i < GAME_MODE_SIZE; i++) {
        draw_button(&mode_buttons[i]);
    }
}

static void cam_posclamp(Camera2D *cam, Vector2 pos)
{
    float halfx = cam->offset.x / cam->zoom;
    float halfy = cam->offset.y / cam->zoom;

    cam->target.x = clamp(pos.x, halfx, WIDTH  - halfx);
    cam->target.y = clamp(pos.y, halfy, HEIGHT - halfy);
}

static void cam_reset(Camera2D *cam)
{
    *cam = (Camera2D) {
        .offset = CAM_BASE_OFFSET,
        .rotation = 0,
        .target = vec2(WIDTH / 2.0f, HEIGHT / 2.0f),
        .zoom = 1.0f,
    };
}

static void start_game(Game *game)
{
    left_score = right_score = 0;
    cam_reset(&game->cam);
    game->resetting_ball = false;
    game->cam_following_ball = false;
    game->wait_time = 0;
    init_ball(&game->ball, game->ball_pick, BALL_START_POS, BALL_GAME_SIZE);
    init_player(&game->players[PLAYER_SIDE_LEFT], game->char_picks[PLAYER_SIDE_LEFT], PLAYER_SIDE_LEFT, PLAYER_LEFT_START_POS, PLAYER_GAME_SIZE, SPEED, JMP_FORCE);
    init_player(&game->players[PLAYER_SIDE_RIGHT], game->char_picks[PLAYER_SIDE_RIGHT], PLAYER_SIDE_RIGHT, PLAYER_RIGHT_START_POS, PLAYER_GAME_SIZE, SPEED, JMP_FORCE);

    if (mode_issingle(game))
        game->players[PLAYER_SIDE_RIGHT].is_bot = true;
    else
        game->players[PLAYER_SIDE_RIGHT].is_bot = false;
}

static void reset_game(Game *game)
{
    Player *a  = &game->players[0];
    Player *b  = &game->players[1];
    Ball *ball = &game->ball;

    ascir(ball->p).pos = BALL_START_POS;
    ball->p.velo = vec2zero;
    ball->rot = 0;
    ball->hitleft_trail = 0;

    ascir(a->p).pos = PLAYER_LEFT_START_POS;
    a->p.velo = vec2zero;
    ascir(b->p).pos = PLAYER_RIGHT_START_POS;
    b->p.velo = vec2zero;
}

static int move_picker(int pick, int dx, int dy, int cols, int rows)
{
    int col = pick % cols;
    int row = pick / cols;
    if (pick == -1)
        col = row = -1;

    int new_col = col + dx;
    int new_row = row + dy;

    if (pick == -1 && new_col == -1) {
        new_col = 0;
    }
    if (pick == -1 && new_row == -1) {
        new_row = 0;
    }

    if (new_col < 0)
        new_col = cols - 1;
    else if (new_col > cols - 1)
        new_col = 0;

    if (new_row < 0)
        new_row = rows - 1;
    else if (new_row > rows - 1)
        new_row = 0;

    return new_col + new_row * cols;
}

static void draw_menu(Game *game)
{
    if (menu_ismain(game)) {
        draw_main_buttons(game, &lres, &rres);
    } else if (menu_issettings(game)) {
        draw_settings_buttons(game, &lres, &rres);
    } else if (menu_ismode(game)) {
        draw_mode_buttons(game, &lres, &rres);
    } else if (menu_isballpick(game)) {
        int *ball_pick = &game->ball_pick;

        int left_x  = lres.press_axis.x;
        int left_y  = lres.press_axis.y;
        int right_x = 0;
        int right_y = 0;

        if (mode_ismulti(game)) {
            right_x = rres.press_axis.x;
            right_y = rres.press_axis.y;
        }

        if (left_x || right_x) {
            *ball_pick += left_x + right_x;
            if (*ball_pick < 0) {
                *ball_pick = BALL_SIZE - 1;
            } else if (*ball_pick >= BALL_SIZE) {
                *ball_pick = 0;
            }
        }
        
        Vector2 size = vec2(192, 192);
        Vector2 pos  = vec2(80, (HEIGHT - size.y * 2) / 2);
        for (int i = 0; i < BALL_SIZE; i++) {
            Rectangle src  = {
                BALL_SPRITE_SIZE * i,
                0,
                BALL_SPRITE_SIZE,
                BALL_SPRITE_SIZE
            };
            Rectangle dest = {
                pos.x + 12,
                pos.y + 12,
                size.x - 24,
                size.y - 24
            };
            Rectangle frame = rec(pos.x - 8, pos.y - 8, size.x + 16, size.y + 16);
            Color c = WHITE;
            if (*ball_pick == i)
                c = GREEN;

            DrawRectangleRec(frame, c);
            DrawTexturePro(ball_tex, src, dest, vec2(0, 0), 0, WHITE);
            pos.x += size.x + 32;
            /*
            if ((i + 1) % cols == 0) {
                pos.x = 80;
                pos.y += size.y + 32;
            }
            */
        }

        if (lres.forw_btn || rres.forw_btn) {
            if (*ball_pick != -1) {
                reset_charpicks(game);
                game->menu_state = MENU_STATE_CHARPICK;
            }
        }
    } else if (menu_ischarpick(game)) {
        int *left_pick  = &game->char_picks[PLAYER_SIDE_LEFT];
        int *right_pick = &game->char_picks[PLAYER_SIDE_RIGHT];

        static const int cols = 2;
        static const int rows = 4;

        int left_x  = lres.press_axis.x;
        int left_y  = lres.press_axis.y;
        int right_x = 0;
        int right_y = 0;

        if (mode_ismulti(game)) {
            right_x = rres.press_axis.x;
            right_y = rres.press_axis.y;
        }

        if (left_x || left_y)
            *left_pick  = move_picker(*left_pick, left_x, left_y, cols, rows);
        if (right_x || right_y)
            *right_pick = move_picker(*right_pick, right_x, right_y, cols, rows);
        
        static const float gap = 32;
        Vector2 size = vec2(128, 128);
        Vector2 start_pos  = vec2(WIDTH / 2.0f - size.x - gap / 2.0f, gap);
        //DrawLine(WIDTH / 2.0f, 0, WIDTH / 2.0f, HEIGHT, RED);
        Vector2 pos = start_pos;
        for (int i = 0; i < PLAYER_SIZE; i++) {
            Rectangle src  = {
                PLAYER_SPRITE_SIZE * i,
                0,
                PLAYER_SPRITE_SIZE,
                PLAYER_SPRITE_SIZE
            };
            Rectangle dest = {
                pos.x + 12,
                pos.y + 12,
                size.x - 24,
                size.y - 24
            };
            Rectangle frame = rec(pos.x - 8, pos.y - 8, size.x + 16, size.y + 16);
            Color c = WHITE;
            if (*left_pick == i)
                c = RED;
            else if (mode_ismulti(game) && *right_pick == i)
                c = BLUE;

            if (mode_ismulti(game) && *left_pick == i && i == *right_pick) {
                DrawTriangle(
                    vec2(frame.x, frame.y),
                    vec2(frame.x, frame.y + frame.height),
                    vec2(frame.x + frame.width, frame.y + frame.height),
                    RED
                );
                DrawTriangle(
                    vec2(frame.x, frame.y),
                    vec2(frame.x + frame.width, frame.y + frame.height),
                    vec2(frame.x + frame.width, frame.y),
                    BLUE
                );
            } else {
                DrawRectangleRec(frame, c);
            }
            DrawTexturePro(player_tex, src, dest, vec2(0, 0), 0, WHITE);
            pos.x += size.x + gap;
            if ((i + 1) % cols == 0) {
                pos.x = start_pos.x;
                pos.y += size.y + gap;
            }

            Vector2 bsize = vec2(392, 640);
            if (*left_pick == i) {
                Rectangle brec = rec(32, 32, bsize.x, bsize.y);
                DrawRectangleRec(brec, WHITE);
                DrawTexturePro(player_tex,
                               rec(
                                   PLAYER_SPRITE_SIZE * i,
                                   0,
                                   PLAYER_SPRITE_SIZE,
                                   PLAYER_SPRITE_SIZE
                               ),
                               rec(brec.x, brec.y, brec.width, brec.width),
                               vec2zero,
                               0,
                               WHITE
                              );
                Vector2 tpos = vec2(brec.x, brec.y + brec.width);
                draw_text(char_info_boxes[i].name, tpos, 32, BLACK);
            }
            if (*right_pick == i) {
                Rectangle brec = rec(WIDTH - bsize.x - 32, 32, bsize.x, bsize.y);
                DrawRectangleRec(brec, WHITE);
                DrawTexturePro(player_tex,
                               rec(
                                   PLAYER_SPRITE_SIZE * i,
                                   0,
                                   -PLAYER_SPRITE_SIZE,
                                   PLAYER_SPRITE_SIZE
                               ),
                               rec(brec.x, brec.y, brec.width, brec.width),
                               vec2zero,
                               0,
                               WHITE
                              );
            }
        }

        if (lres.forw_btn || rres.forw_btn) {
            int start = false;
            if (mode_issingle(game)) {
                if (*left_pick != -1) {
                    *right_pick = rand() % PLAYER_SIZE;
                    start = true;
                }
            } else if (*left_pick != -1 && *right_pick != -1) {
                start = true;
            }
            if (start) {
                game->game_state = GAME_STATE_RUN;
                start_game(game);
            }
        }
    }
}

static void draw_gameui(Game *game)
{
    if (show_fps)
        draw_text(TextFormat("%d", GetFPS()), vec2(8, 8), 32, BLACK);

    draw_text(TextFormat("%d", left_score), vec2(64, 32), 64, BLACK);
    draw_text(TextFormat("%d", right_score), vec2(WIDTH - 8 - 64 - 32, 32), 64, BLACK);

    /* bars */
    Player *a = &game->players[0];
    Player *b = &game->players[1];
    Vector2 lbar_pos = vec2(128, 32);
    Vector2 rbar_pos = vec2(WIDTH - lbar_pos.x, lbar_pos.y);
    Vector2 bar_size = vec2(256, 32);
    float ratio_a, ratio_b;
    if (a->super.active) {
        if (a->super.charged) {
            ratio_a = 1.0f;
        } else if (a->super.being_used) {
            ratio_a = a->super.use_time / a->super.maxuse_time;
        } else {
            ratio_a = a->super.chr_time / a->super.maxchr_time;
        }
    }
    if (b->super.active) {
        if (b->super.charged) {
            ratio_b = 1.0f;
        } else if (b->super.being_used) {
            ratio_b = b->super.use_time / b->super.maxuse_time;
        } else {
            ratio_b = b->super.chr_time / b->super.maxchr_time;
        }
    }

    if (a->super.active) {
        DrawRectangleV(lbar_pos, bar_size, BLACK);
        DrawRectangleV(
            lbar_pos,
            vec2(bar_size.x * ratio_a, bar_size.y),
            RED
        );
    }

    if (b->super.active) {
        DrawRectangleV(vec2(rbar_pos.x - bar_size.x, rbar_pos.y), bar_size, BLACK);
        rbar_pos.x -= bar_size.x * ratio_b;
        DrawRectangleV(
            rbar_pos,
            vec2(bar_size.x * ratio_b, bar_size.y),
            BLUE
        );
    }
}

void init_game(Game *game)
{
    rtex = LoadRenderTexture(WIDTH, HEIGHT);
    srand(time(NULL));
    *game = (Game) {0};
    init_soundmanager(&game->sm);
    /* game */
    game->game_state = GAME_STATE_MENU;
    /* menu */
    init_main_buttons();
    init_settings_buttons();
    init_mode_buttons();
    /* picking */
    reset_picks(game);
    /* camera */
    cam_reset(&game->cam);
    /* textures */
    load_font("assets/joystixmono.otf");
    init_map(&game->map);
    player_tex = LoadTexture("assets/player.png");
    ball_tex = LoadTexture("assets/ball.png");
    /* ball */
    /* players controls */
    /* gamepad */
    game->controls[PLAYER_SIDE_LEFT]  = control_left;
    game->controls[PLAYER_SIDE_RIGHT] = control_right;
    load_settings(game);
    /* bars */
    game->bars[PLAYER_SIDE_LEFT]  = pobject_staticrec(rec(0, BAR_POS_Y, BAR_WIDTH, BAR_HEIGHT));
    game->bars[PLAYER_SIDE_RIGHT] = pobject_staticrec(rec(WIDTH - BAR_WIDTH, BAR_POS_Y, BAR_WIDTH, BAR_HEIGHT));
    /* borders */
    game->borders[0] = pobject_staticrec(rec(0, 0 - BOR_THICK, WIDTH, BOR_THICK));
    game->borders[1] = pobject_staticrec(rec(0, GROUND, WIDTH, BOR_THICK));
    game->borders[2] = pobject_staticrec(rec(0 - BOR_THICK, 0, BOR_THICK, GROUND));
    game->borders[3] = pobject_staticrec(rec(WIDTH, 0, BOR_THICK, GROUND));
}

void draw_game(Game *game)
{
    if (IsKeyPressed(KEY_ONE) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) {
        game->map.type = MAP_STREET;
    } else if (IsKeyPressed(KEY_TWO) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
        game->map.type = MAP_BEACH;
    }
    BeginTextureMode(rtex);
        ClearBackground(WHITE);
        BeginMode2D(game->cam);
            switch (game->map.type) {
            case MAP_STREET:
                //DrawTextureEx(game->map.street.bg, vec2(0, 0), 0, SCALE, WHITE);
                DrawTexturePro(game->map.street.bg, rec(game->map.street.cur_frame * 320, 0, 320, 180), rec(0, 0, WIDTH, HEIGHT), vec2(0, 0), 0, WHITE);
                break;
            case MAP_BEACH:
                DrawTextureEx(game->map.beach.bg, vec2(0, 0), 0, SCALE, WHITE);
                break;
            }
            if (game_onmenu(game)) {
                DrawTextureEx(game->map.goal, vec2(0, 0), 0, SCALE, WHITE);
                DrawTextureEx(game->map.front, vec2(0, 0), 0, SCALE, WHITE);
                //DrawRectangleRec(game->bars[0].as.r, color(255, 0, 0, 100));
                draw_menu(game);
            } else {
                DrawTextureEx(game->map.goal, vec2(0, 0), 0, SCALE, WHITE);
                draw_player(&game->players[0], player_tex);
                draw_player(&game->players[1], player_tex);
                draw_ball(&game->ball, ball_tex);
                //draw_recs(game->borders, 4);
                DrawTextureEx(game->map.front, vec2(0, 0), 0, SCALE, WHITE);
                switch (game->map.type) {
                case MAP_STREET:
                    break;
                case MAP_BEACH:
                    DrawTextureEx(game->map.beach.tree, vec2(0, 0), 0, SCALE, WHITE);
                    break;
                }
            }
        EndMode2D();

        if (game_isrun(game))
            draw_gameui(game);
    EndTextureMode();

    //DrawTextureRec(rtex.texture, rec(0, 0, WIDTH, -HEIGHT), vec2(0, 0), WHITE);
    DrawTexturePro(rtex.texture, rec(0, 0, WIDTH, -HEIGHT), rec(0, 0, GetScreenWidth(), GetScreenHeight()), vec2(0, 0), 0, WHITE);
}

void update_game(Game *game, float dt)
{
    lres = get_playerinputresult(&game->controls[PLAYER_SIDE_LEFT]);
    rres = get_playerinputresult(&game->controls[PLAYER_SIDE_RIGHT]);
    int quit = IsKeyPressed(KEY_ESCAPE) || lres.back_btn || rres.back_btn;

    if (quit) {
        switch (game->game_state) {
        case GAME_STATE_MENU:
            switch (game->menu_state) {
            case MENU_STATE_MAIN:
                exit(0);
            case MENU_STATE_PLAYMODE:
                game->menu_state = MENU_STATE_MAIN;
                break;
            case MENU_STATE_BALLPICK:
                game->menu_state = MENU_STATE_PLAYMODE;
                break;
            case MENU_STATE_CHARPICK:
                game->menu_state = MENU_STATE_BALLPICK;
                break;
            case MENU_STATE_SETTINGS:
                save_settings(game);
                game->menu_state = MENU_STATE_MAIN;
                break;
            }
        case GAME_STATE_RUN: case GAME_STATE_PAUSE:
            cam_reset(&game->cam);
            game->game_state = GAME_STATE_MENU;
            //game->menu_state = MENU_STATE_PICK;
            break;
        }
    }

    if (IsKeyPressed(KEY_F1))
        show_fps = !show_fps;
    if (IsKeyPressed(KEY_F11)) {
        static int fullscreen = false;
        int width = GetMonitorWidth(0);
        int height = GetMonitorHeight(0);
        if (!fullscreen) {
            SetWindowSize(width, height);
            SetWindowPosition(0, 0);
        } else {
            SetWindowSize(WIDTH, HEIGHT);
            SetWindowPosition((width - WIDTH) / 2.0f, (height - HEIGHT) / 2.0f);
        }

        fullscreen = !fullscreen;
    }

    switch (game->map.type) {
    case MAP_STREET:
        game->map.street.time += dt;
        if (game->map.street.time >= game->map.street.frame_time) {
            game->map.street.time = 0.0f;
            game->map.street.cur_frame = (game->map.street.cur_frame + 1) % game->map.street.max_frame;
        }
        break;
    case MAP_BEACH:
        break;
    }

    if (game_onmenu(game)) {
        return;
    }
    if (IsKeyPressed(KEY_P)) {
        game->game_state = game_onpause(game) ? GAME_STATE_RUN : GAME_STATE_PAUSE;
    }
    if (game_onpause(game)) {
        return;
    }

    Player *a  = &game->players[0];
    Player *b  = &game->players[1];
    Ball *ball = &game->ball;

    int scored = false;
    if (
        !game->cam_following_ball &&
        ascir(ball->p).pos.x + ascir(ball->p).radius < asrec(game->bars[0]).x + asrec(game->bars[0]).width &&
        ascir(ball->p).pos.y - ascir(ball->p).radius > asrec(game->bars[0]).y
    ) {
        right_score++;
        scored = true;
    } else if (
        !game->cam_following_ball &&
        ascir(ball->p).pos.x - ascir(ball->p).radius > asrec(game->bars[1]).x &&
        ascir(ball->p).pos.y - ascir(ball->p).radius > asrec(game->bars[1]).y
    ) {
        left_score++;
        scored = true;
    }

    if (scored) {
        game->cam_following_ball = true;
        play_cheer(&game->sm);
    }

    if (game->cam_following_ball) {
        Vector2 ball_lerped_pos;
        float target_zoom = 3.0f;
        ball_lerped_pos.x = lerp(game->cam.target.x, ascir(game->ball.p).pos.x, dt * 5);
        ball_lerped_pos.y = lerp(game->cam.target.y, ascir(game->ball.p).pos.y, dt * 5);
        game->cam.zoom = lerp(game->cam.zoom, target_zoom, dt * 2.5f);
        game->cam.zoom = clamp(game->cam.zoom, 1.0f, 3.0f);
        cam_posclamp(&game->cam, ball_lerped_pos);
        if ((game->wait_time += dt) > GAME_ROUND_MAX_WAIT_TIME) {
            game->cam_following_ball = false;
            game->wait_time = 0;
            game->cam.zoom = 1.0f;
            game->cam.target = vec2(WIDTH / 2.0f, HEIGHT / 2.0f);
            reset_game(game);
        }
    }
    if (game->resetting_ball) {
        ascir(ball->p).pos.x = lerp(ascir(ball->p).pos.x, BALL_START_POS.x, dt * 5);
        ascir(ball->p).pos.y = lerp(ascir(ball->p).pos.y, BALL_START_POS.y, dt * 5);
        ball->p.velo = vec2zero;
        if ((game->wait_time += dt) > GAME_ROUND_MAX_WAIT_TIME) {
            game->resetting_ball = false;
            game->wait_time = 0;
        }
    }
    if (game->shaking) {
        game->shake_time += dt;
        static const Vector2 max_offset = vec2(16, 8);
        game->cam.offset.x = CAM_BASE_OFFSET.x + sinf(game->shake_time * 80) * max_offset.x;
        game->cam.offset.y = CAM_BASE_OFFSET.y + sinf(game->shake_time * 60) * max_offset.y;
        if (game->shake_time >= SHAKE_MAX_TIME) {
            game->shake_time = 0;
            game->shaking = false;
            cam_reset(&game->cam);
        }
    }

    if (IsKeyPressed(KEY_F5)) {
        reset_game(game);
    }

    update_player(a, lres, game, dt);
    update_player(b, rres, game, dt);

    int ball_push =
        check_cir_coll(ascir(ball->p), ascir(a->p), NULL, NULL) &&
        check_cir_coll(ascir(ball->p), ascir(b->p), NULL, NULL);

    update_ball(&game->ball, game, dt);

    handle_coll(&a->p, &b->p, dt);

    int ball_hit_times = 0;
    ball_hit_times += handle_coll(&ball->p, &a->p, dt);
    ball_hit_times += handle_coll(&ball->p, &b->p, dt);

    if (ball_push) {
        ball->p.velo.y = -70.0f * (sqrtf(a->p.mass) + sqrtf(b->p.mass));
        ball->p.velo.x = clamp(ball->p.velo.x, -100, 100);
    }

    /* FIX */
    a->p.on_ground = b->p.on_ground = 0;
    for (int i = 0; i < 4; i++) {
        ball_hit_times += handle_coll(&ball->p, &game->borders[i], dt);
        handle_coll(&a->p, &game->borders[i], dt);
        handle_coll(&b->p, &game->borders[i], dt);
    }

    int hit_bar = false;
    for (int i = 0; i < 2; i++) {
        int res = handle_coll(&ball->p, &game->bars[i], dt);
        if (res) {
            if (vec2len(ball->p.velo) > 20.0f)
                play_hitbar(&game->sm);
            hit_bar = res;
        }
        handle_coll(&a->p, &game->bars[i], dt);
        handle_coll(&b->p, &game->bars[i], dt);
    }

    if (ball_hit_times > 0) {
        ball->hitleft_trail -= ball_hit_times;
    }

    if (hit_bar && fabs(ball->p.velo.x) < 4.0f) {
        ball->stop_time += dt;
        ball->hitleft_trail -= hit_bar;
    } else {
        ball->stop_time = 0;
    }
    if (ball->stop_time >= BALL_MAX_STOP_TIME) {
        game->resetting_ball = true;
    }
}
