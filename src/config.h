#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

#define DEBUG 1
#define WIDTH  1280
#define HEIGHT 720
#define TITLE  "Head Ball"
#define FPS 120
#define GRAVITY 2200
#define SCALE 4
#define GROUND (HEIGHT - 20 /* ground in sprite */ * SCALE)

#endif /* CONFIG_H */
