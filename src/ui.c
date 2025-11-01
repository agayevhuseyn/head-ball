#include "ui.h"
#include "font.h"
#include "macros.h"
#include <raylib.h>


void draw_button(Button *b)
{
    Color c = b->hovered ? LIGHTGRAY : WHITE;
    DrawRectangleRec(b->rec, c);
    draw_fit_text(
        b->text,
        vec2(b->rec.x + b->rec.width / 2.0f, b->rec.y + b->rec.height / 2.0f),
        40,
        BLACK
    );
}
