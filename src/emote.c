#include "emote.h"
#include "macros.h"
#include "font.h"
#include <stdio.h> /* fopen, fclose, fputc, fputs */
#include <string.h> /* strcpy */


#define DIR "assets/"

#define SPEED 8.0f
#define FRAME_SIZE vec2(16, 16)
#define OPEN_TIME 0.5f
#define WAIT_TIME 0.9f
#define CLOSE_TIME 0.35f


#define EMOTE_TEXT_SIZE 4
#define EMOTE_TEXT_BUFSIZ 32
static char emote_texts[2][EMOTE_TEXT_SIZE][EMOTE_TEXT_BUFSIZ];
static const char *emote_text_samples[EMOTE_TEXT_SIZE] = {
    "GOOD LUCK",
    "HAHAHA",
    "OOPS..",
    "GOOD GAME"
};


void init_emotemanager(EmoteManager *em, Player players[2])
{
    *em = (EmoteManager) {0};

    em->emotebox_tex = LoadTexture(DIR"emotebox.png");

    em->states[0].p = &players[0];
    em->states[1].p = &players[1];

    const char *file_name = "emotes.txt";
    FILE *fp = fopen("emotes.txt", "r");
    if (fp == NULL) {
        fp = fopen("emotes.txt", "w");
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 4; j++) {
                fputs(emote_text_samples[j], fp);
                fputc('\n', fp);
                strcpy(emote_texts[i][j], emote_text_samples[j]);
            }
            fputc('\n', fp);
        }
        fclose(fp);
    } else {
        char c;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < EMOTE_TEXT_SIZE; j++) {
                int cpos = 0;
                while (cpos < EMOTE_TEXT_BUFSIZ - 1 && (c = fgetc(fp)) != '\n' && c != EOF) {
                    emote_texts[i][j][cpos++] = c;
                }
                emote_texts[i][j][cpos] = '\0';
            }
            while ((c = fgetc(fp)) == ' ' || c == '\n');
            if (c != EOF)
                ungetc(c, fp);
        }
        fclose(fp);
    }
}

void send_emote(EmoteManager *em, int side, int i)
{
    if (em->states[side].active)
        return;

    em->states[side].active = true;
    em->states[side].i = i;
    em->states[side].state = EMOTEBOX_OPENING;
}

static float ease_out_back(float t)
{
    float c1 = 1.70158f;
    float c3 = c1 + 1.0f;
    return 1 + c3 * (t - 1) * (t - 1) * (t - 1)
             + c1 * (t - 1) * (t - 1);
}

#define ease_in(t) ((t) * (t))

void update_emote(EmoteManager *em, float dt)
{
    for (int i = 0; i < 2; i++) {
        if (!em->states[i].active)
            continue;

        em->states[i].timer += dt;
        em->states[i].frame_timer += dt;

        switch (em->states[i].state) {
        case EMOTEBOX_OPENING: {
            float t = clamp(em->states[i].timer / OPEN_TIME, 0, 1);

            float s = ease_out_back(t);
            em->states[i].scale = vec2(s, s);

            if (em->states[i].timer >= OPEN_TIME) {
                em->states[i].timer = 0;
                em->states[i].state = EMOTEBOX_WAITING;
                em->states[i].scale = vec2(1, 1);
            }
        } break;
        case EMOTEBOX_WAITING:
            if (em->states[i].timer >= WAIT_TIME) {
                em->states[i].timer = 0;
                em->states[i].state = EMOTEBOX_CLOSING;
            }
            break;
        case EMOTEBOX_CLOSING: {
            float t = clamp(em->states[i].timer / CLOSE_TIME, 0, 1);
            
            float s = 1.0f - ease_in(t);
            em->states[i].scale = vec2(s, s);

            if (em->states[i].timer >= CLOSE_TIME) {
                em->states[i].timer = 0;
                em->states[i].active = false;
            }
        } break;
        }
    }
}

void draw_emote(EmoteManager *em)
{
    for (int i = 0; i < 2; i++) {
        if (!em->states[i].active)
            continue;

        Player *p = em->states[i].p;
        Texture2D *tex = &em->emotebox_tex;
        int side = p->side;
        Rectangle box_src = rec(0, 0, (side == PLAYER_SIDE_LEFT ? 1 : -1) * tex->width, tex->height);
        float def_scaler = 8;
        Vector2 box_size = vec2(
            tex->width  * def_scaler,
            tex->height * def_scaler
        );
        Vector2 box_scale = vec2(em->states[i].scale.x, em->states[i].scale.y);
        Rectangle box_dest = rec(
            ascir(p->p).pos.x + (side == PLAYER_SIDE_LEFT ? 1 : -1) * (ascir(p->p).radius + (side == PLAYER_SIDE_LEFT ? 0 : box_size.x)),
            ascir(p->p).pos.y - ascir(p->p).radius * 2,
            box_size.x * box_scale.x,
            box_size.y * box_scale.y
        );

        DrawTexturePro(*tex, box_src, box_dest, vec2(0, 0), 0, WHITE);

        Rectangle text_rec = rec(
            box_dest.x + box_dest.width * 0.1,
            box_dest.y + box_dest.height * 0.05f,
            box_dest.width  * 0.8f, 
            box_dest.height * 0.8f
        );

        draw_fit_text(emote_texts[side][em->states[i].i], text_rec, BLACK);
    }
}
