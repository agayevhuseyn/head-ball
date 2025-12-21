#include "sound.h"


#define DIR "assets/sounds/"


void init_soundmanager(SoundManager *sm)
{
    *sm = (SoundManager) {0};
    for (int i = 0; i < SOUND_HITBAR_SIZE; i++)
        sm->hitbar[i] = LoadSound(DIR"hitbar.wav");

    for (int i = 0; i < SOUND_JUMP_SIZE; i++)
        sm->jump[i] = LoadSound(DIR"jump.wav");

    for (int i = 0; i < SOUND_BOMB_SIZE; i++)
        sm->bomb[i] = LoadSound(DIR"bomb.wav");

    for (int i = 0; i < SOUND_KICK_SIZE; i++)
        sm->kick[i] = LoadSound(DIR"kick.wav");

    for (int i = 0; i < SOUND_FIRE_SIZE; i++)
        sm->fire[i] = LoadSound(DIR"fire.wav");

    for (int i = 0; i < SOUND_ALIEN_SIZE; i++)
        sm->alien[i] = LoadSound(DIR"alien.wav");

    sm->sizzle = LoadSound(DIR"sizzle.wav");
    sm->cheer = LoadSound(DIR"cheer.wav");

    for (int i = 0; i < SOUND_EGGHEAD_SIZE; i++)
        sm->egghead[i] = LoadSound(DIR"shock.wav");

    for (int i = 0; i < SOUND_STUN_SIZE; i++)
        sm->stun[i] = LoadSound(DIR"stun.wav");

    for (int i = 0; i < SOUND_HACK_SIZE; i++)
        sm->hack[i] = LoadSound(DIR"hack.wav");

    for (int i = 0; i < SOUND_SLOWMO_SIZE; i++)
        sm->slowmo[i] = LoadSound(DIR"slowmo.wav");

    for (int i = 0; i < SOUND_SLOWMOEND_SIZE; i++)
        sm->slowmoend[i] = LoadSound(DIR"slowmoend.wav");

    for (int i = 0; i < SOUND_JOKER_SIZE; i++)
        sm->joker[i] = LoadSound(DIR"joker.wav");

    for (int i = 0; i < SOUND_EMOTE_SIZE; i++)
        sm->emote[i] = LoadSound(DIR"emote.wav");
}

void play_hitbar(SoundManager *sm)
{
    PlaySound(sm->hitbar[sm->hitbar_i]);
    sm->hitbar_i = (sm->hitbar_i + 1) % SOUND_HITBAR_SIZE;
}

void play_jump(SoundManager *sm)
{
    PlaySound(sm->jump[sm->jump_i]);
    sm->jump_i = (sm->jump_i + 1) % SOUND_JUMP_SIZE;
}

void play_bomb(SoundManager *sm)
{
    PlaySound(sm->bomb[sm->bomb_i]);
    sm->bomb_i = (sm->bomb_i + 1) % SOUND_BOMB_SIZE;
}

void play_kick(SoundManager *sm)
{
    PlaySound(sm->kick[sm->kick_i]);
    sm->kick_i = (sm->kick_i + 1) % SOUND_KICK_SIZE;
}

void play_fire(SoundManager *sm)
{
    PlaySound(sm->fire[sm->fire_i]);
    sm->fire_i = (sm->fire_i + 1) % SOUND_FIRE_SIZE;
}

void play_alien(SoundManager *sm)
{
    PlaySound(sm->alien[sm->alien_i]);
    sm->alien_i = (sm->alien_i + 1) % SOUND_ALIEN_SIZE;
}

void play_sizzle(SoundManager *sm)
{
    if (!IsSoundPlaying(sm->sizzle)) {
        PlaySound(sm->sizzle);
    }
}

void play_cheer(SoundManager *sm)
{
    if (!IsSoundPlaying(sm->cheer)) {
        PlaySound(sm->cheer);
    }
}

void play_egghead(SoundManager *sm)
{
    PlaySound(sm->egghead[sm->egghead_i]);
    sm->egghead_i = (sm->egghead_i + 1) % SOUND_EGGHEAD_SIZE;
}

void play_stun(SoundManager *sm)
{
    PlaySound(sm->stun[sm->stun_i]);
    sm->stun_i = (sm->stun_i + 1) % SOUND_STUN_SIZE;
}

void play_hack(SoundManager *sm)
{
    PlaySound(sm->hack[sm->hack_i]);
    sm->hack_i = (sm->hack_i + 1) % SOUND_HACK_SIZE;
}

void play_slowmo(SoundManager *sm)
{
    PlaySound(sm->slowmo[sm->slowmo_i]);
    sm->slowmo_i = (sm->slowmo_i + 1) % SOUND_SLOWMO_SIZE;
}

void play_slowmoend(SoundManager *sm)
{
    PlaySound(sm->slowmoend[sm->slowmoend_i]);
    sm->slowmoend_i = (sm->slowmoend_i + 1) % SOUND_SLOWMOEND_SIZE;
}

void play_joker(SoundManager *sm)
{
    PlaySound(sm->joker[sm->joker_i]);
    sm->joker_i = (sm->joker_i + 1) % SOUND_JOKER_SIZE;
}

void play_emote(SoundManager *sm)
{
    PlaySound(sm->emote[sm->emote_i]);
    sm->emote_i = (sm->emote_i + 1) % SOUND_EMOTE_SIZE;
}
