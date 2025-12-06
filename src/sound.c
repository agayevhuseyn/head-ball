#include "sound.h"


#define DIR "assets/sounds/"


void init_soundmanager(SoundManager *sm)
{
    *sm = (SoundManager) {0};
    for (int i = 0; i < SOUND_HITBAR_SIZE; i++)
        sm->hitbar[i] = LoadSound(DIR"hitbar.wav");
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
