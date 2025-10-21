#ifndef FONT_H
#define FONT_H


#include <raylib.h>


void load_font(const char *path);
Font get_font();

#define draw_text(t, p, s, c) DrawTextEx( \
    get_font(), \
    (t), \
    (p), \
    (s), \
    0, \
    (c) \
)


#endif /* FONT_H */
