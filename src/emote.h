#ifndef EMOTE_H
#define EMOTE_H


#include "player.h"
#include "sound.h"

typedef enum {
    EMOTEBOX_OPENING,
    EMOTEBOX_WAITING,
    EMOTEBOX_CLOSING,
} EmoteBoxState;

typedef struct {
    Texture2D emotebox_tex;
    struct {
        Player *p;
        int active;
        float timer;
        EmoteBoxState state;
        int i;
        float frame_timer;
        int frame;
        Vector2 scale;
        const char *emote_texts[2][4];
    } states[2];
} EmoteManager;


void init_emotemanager(EmoteManager *em, Player players[2]);
void send_emote(EmoteManager *em, SoundManager *sm, int side, int i);
void update_emote(EmoteManager *em, float dt);
void draw_emote(EmoteManager *em);


#endif /* EMOTE */
