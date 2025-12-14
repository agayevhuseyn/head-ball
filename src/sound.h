#ifndef SOUND_H
#define SOUND_H


#include <raylib.h>


#define SOUND_HITBAR_SIZE 1
#define SOUND_JUMP_SIZE 2
#define SOUND_BOMB_SIZE 1
#define SOUND_KICK_SIZE 2
#define SOUND_FIRE_SIZE 2
#define SOUND_ALIEN_SIZE 2
#define SOUND_EGGHEAD_SIZE 2
#define SOUND_STUN_SIZE 2
#define SOUND_HACK_SIZE 2
#define SOUND_SLOWMO_SIZE 2
#define SOUND_SLOWMOEND_SIZE 2

typedef struct {
} SoundGroup;

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
    Sound alien[SOUND_ALIEN_SIZE];
    int alien_i;
    Sound sizzle;
    Sound cheer;
    Sound egghead[SOUND_EGGHEAD_SIZE];
    int egghead_i;
    Sound stun[SOUND_STUN_SIZE];
    int stun_i;
    Sound hack[SOUND_STUN_SIZE];
    int hack_i;
    Sound slowmo[SOUND_STUN_SIZE];
    int slowmo_i;
    Sound slowmoend[SOUND_STUN_SIZE];
    int slowmoend_i;
} SoundManager;


void init_soundmanager(SoundManager *sm);
void play_hitbar(SoundManager *sm);
void play_jump(SoundManager *sm);
void play_bomb(SoundManager *sm);
void play_kick(SoundManager *sm);
void play_fire(SoundManager *sm);
void play_alien(SoundManager *sm);
void play_sizzle(SoundManager *sm);
void play_cheer(SoundManager *sm);
void play_egghead(SoundManager *sm);
void play_stun(SoundManager *sm);
void play_hack(SoundManager *sm);
void play_slowmo(SoundManager *sm);
void play_slowmoend(SoundManager *sm);


#endif /* SOUND_H */
