#ifndef INPUT_H
#define INPUT_H


#include <raylib.h>


typedef struct {
    int gamepad_id;
    struct {
        int left_btn;
        int right_btn;
        int up_btn;
        int down_btn;
        int super_btn;
        int strhit_btn;
        int uphit_btn;
    } keyboard;
    struct {
        int super_btn;
        int strhit_btn;
        int uphit_btn;
    } gamepad;
} PlayerControl;


typedef struct {
    Vector2 faxis;
    struct {
        int x;
        int y;
    } iaxis;
    struct {
        int x;
        int y;
    } press_axis;
    int super_btn;
    int strhit_btn;
    int uphit_btn;
} PlayerInputResult;


PlayerInputResult get_playerinputresult(PlayerControl *pc);


#endif /* INPUT_H */
