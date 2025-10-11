#ifndef MACROS_H
#define MACROS_H


#define vec2(x, y) (Vector2) {x, y}
#define color(r, g, b, a) (Color) {r, g, b, a}
#define rec(x, y, w, h) (Rectangle) {x, y, w, h}
#define clamp(val, min, max) (val < min ? min : val > max ? max : val)


#endif /* MACROS_H */
