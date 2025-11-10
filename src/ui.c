#include "ui.h"
#include "font.h"
#include "macros.h"
#include <raylib.h>
#include <math.h>


void draw_button(Button *b)
{
    Color c = b->hovered ? LIGHTGRAY : WHITE;
    if (b->pressed)
        c = GRAY;

    DrawRectangleRec(b->rec, c);
    draw_fit_text(
        b->text,
        b->rec,
        BLACK
    );
}
