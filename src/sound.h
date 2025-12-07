#ifndef SOUND_H
#define SOUND_H


#include <raylib.h>


#define SOUND_HITBAR_SIZE 1
#define SOUND_JUMP_SIZE 2
#define SOUND_BOMB_SIZE 1
#define SOUND_KICK_SIZE 2
#define SOUND_FIRE_SIZE 2


typedef struct {
    Sound hitbar[SOUND_HITBAR_SIZE];
    int hitbar_i;
    Sound jump[SOUND_JUMP_SIZE];
    int jump_i;
    Sound bomb[SOUND_BOMB_SIZE];
    int bomb_i;
    Sound kick[SOUND_KICK_SIZE];
    int kick_i;
    Sound fire[SOUND_FIRE_SIZE];
    int fire_i;
} SoundManager;


void init_soundmanager(SoundManager *sm);
void play_hitbar(SoundManager *sm);
void play_jump(SoundManager *sm);
void play_bomb(SoundManager *sm);
void play_kick(SoundManager *sm);
void play_fire(SoundManager *sm);


#endif /* SOUND_H */
