#ifndef MAP_H
#define MAP_H


#include <raylib.h>


typedef enum {
    MAP_STREET,
    MAP_BEACH
} MapType;

typedef struct {
    MapType type;
    struct {
        Texture2D bg;
        float frame_time;
    } street;
    struct {
        Texture2D bg;
        Texture2D tree;
    } beach;
    Texture2D front;
} Map;


void init_map(Map *m);


#endif /* MAP_H */
