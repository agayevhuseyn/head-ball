#ifndef MACROS_H
#define MACROS_H


#define vec2(x, y) (Vector2) {x, y}
#define color(r, g, b, a) (Color) {r, g, b, a}
#define rec(x, y, w, h)  (Rectangle) {x, y, w, h}
#define recv(p, s) (Rectangle) {(p).x, (p).y, (s).x, (s).y}
#define clamp(val, min, max) (val < min ? min : val > max ? max : val)
#define vec2zero vec2(0, 0)
#define vec2len(v) (sqrtf((v).x * (v).x + (v).y * (v).y))
#define lerp(a, b, dt) ((a) + ((b) - (a)) * (dt))
#define vec2dist(a, b) vec2len(vec2((b).x - (a).x, (b).y - (a).y))


#endif /* MACROS_H */
