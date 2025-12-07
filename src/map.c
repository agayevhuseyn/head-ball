#include "map.h"


#define DIR "assets/"


void init_map(Map *m)
{
    m->type = MAP_STREET;
    m->goal = LoadTexture(DIR"goal.png");
    m->front = LoadTexture(DIR"front1.png");

    m->street.bg = LoadTexture(DIR"street.png");
    m->street.time = 0.0f;
    m->street.frame_time = 0.72f;
    m->street.cur_frame = 0;
    m->street.max_frame = 4;

    m->beach.bg = LoadTexture(DIR"beach.png");
    m->beach.tree = LoadTexture(DIR"tree.png");
}
