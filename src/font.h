#ifndef FONT_H
#define FONT_H


#include <raylib.h>
#include "macros.h"


void load_font(const char *path);
Font get_font();

#define draw_text(t, p, s, c) DrawTextEx( \
    get_font(), \
    (t), \
    (p), \
    (s), \
    0,   \
    (c)  \
)

#define mes_text(t, sz) MeasureTextEx( \
    get_font(), \
    (t),  \
    (sz), \
    0  \
)

#define draw_fit_text(t, p, s, c) ( \
    draw_text( \
        (t),  \
        vec2( \
            (p).x - mes_text(t, s).x / 2.0f, \
            (p).y - mes_text(t, s).y / 2.0f  \
        ),   \
        (s), \
        (c)  \
    ) \
)



#endif /* FONT_H */
