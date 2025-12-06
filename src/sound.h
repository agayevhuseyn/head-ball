#ifndef SOUND_H
#define SOUND_H


#include <raylib.h>


#define SOUND_HITBAR_SIZE 1


typedef struct {
    Sound hitbar[SOUND_HITBAR_SIZE];
    int hitbar_i;
} SoundManager;


void init_soundmanager(SoundManager *sm);
void play_hitbar(SoundManager *sm);


#endif /* SOUND_H */
