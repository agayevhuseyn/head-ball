#include "map.h"


#define DIR "assets/"


void init_map(Map *m)
{
    m->type = MAP_STREET;
    m->front = LoadTexture(DIR"front.png");

    m->street.bg = LoadTexture(DIR"street.png");
    m->street.frame_time = 0.2f;

    m->beach.bg = LoadTexture(DIR"beach.png");
    m->beach.tree = LoadTexture(DIR"tree.png");
}
