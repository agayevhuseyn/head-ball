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

#define draw_fit_text(t, r, c) do { \
    float font_size = 40; \
    Vector2 text_size = mes_text(t, font_size); \
    float scale_x, scale_y; \
    scale_x = (r).width / text_size.x; \
    scale_y = (r).height / text_size.y; \
    float scale = fminf(scale_x, scale_y); \
    font_size *= scale; \
    text_size = mes_text(t, font_size); \
    draw_text( \
        (t),  \
        vec2( \
            (r).x + ((r).width  - text_size.x) / 2.0f, \
            (r).y + ((r).height - text_size.y) / 2.0f  \
        ),   \
        font_size, \
        (c)  \
    ); \
} while (0)



#endif /* FONT_H */
