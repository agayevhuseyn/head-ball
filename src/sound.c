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
}

void play_hitbar(SoundManager *sm)
{
    static float timer = 0.0f;
    static const float max_time = 0.03f;
    timer += GetFrameTime();
//    if 
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
