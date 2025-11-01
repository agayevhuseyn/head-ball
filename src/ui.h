#ifndef UI_H
#define UI_H


#include <raylib.h>

/* Button */
typedef struct {
    Rectangle rec;
    const char *text;
    int hovered;
} Button;

int is_button_pressed(Button *b);
void draw_button(Button *b);


#endif /* UI_H */
