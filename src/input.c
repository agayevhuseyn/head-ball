#include "input.h"
#include <math.h>


#define MIN_AXIS 0.7f

static Vector2 get_gamepad_ljoystick_press(int id)
{
    static int prev_vals[2][2] = {{0}, {0}}; /* update this when more needed */
    float x = GetGamepadAxisMovement(id, GAMEPAD_AXIS_LEFT_X);
    float y = GetGamepadAxisMovement(id, GAMEPAD_AXIS_LEFT_Y);

    Vector2 res;

    int ix = fabs(x) > MIN_AXIS ? (x > 0 ? 1 : -1) : 0;
    int iy = fabs(y) > MIN_AXIS ? (y > 0 ? 1 : -1) : 0;

    if (ix == prev_vals[id][0]) {
        res.x = 0;
    } else {
        res.x = prev_vals[id][0] = ix;
    }

    if (iy == prev_vals[id][1]) {
        res.y = 0;
    } else {
        res.y = prev_vals[id][1] = iy;
    }

    return res;
}

PlayerInputResult get_playerinputresult(PlayerControl *pc)
{
    PlayerInputResult res = {0};

    if (IsGamepadAvailable(pc->gamepad_id)) {
        int id = pc->gamepad_id;
        float x = GetGamepadAxisMovement(id, GAMEPAD_AXIS_LEFT_X);
        float y = GetGamepadAxisMovement(id, GAMEPAD_AXIS_LEFT_Y);

        res.faxis.x = x;
        res.faxis.y = y;

        Vector2 ljoystick_press = get_gamepad_ljoystick_press(id);
        res.press_axis.x = ljoystick_press.x;
        res.press_axis.y = ljoystick_press.y;

        if (fabs(x) > MIN_AXIS)
            res.iaxis.x = x > 0 ? 1 : -1;
        else
            res.iaxis.x = 0;

        if (fabs(y) > MIN_AXIS)
            res.iaxis.y = y > 0 ? 1 : -1;
        else
            res.iaxis.y = 0;

        if (IsGamepadButtonPressed(id, pc->gamepad.super_btn)) {
            res.super_btn = true;
        }
        if (IsGamepadButtonPressed(id, pc->gamepad.strhit_btn)) {
            res.strhit_btn = true;
        }
        if (IsGamepadButtonPressed(id, pc->gamepad.uphit_btn)) {
            res.uphit_btn = true;
        }
        if (IsGamepadButtonPressed(id, pc->gamepad.forw_btn)) {
            res.forw_btn = true;
        }
        if (IsGamepadButtonPressed(id, pc->gamepad.back_btn)) {
            res.back_btn = true;
        }
    } else {
        res.iaxis.x = res.faxis.x = IsKeyDown(pc->keyboard.right_btn) - IsKeyDown(pc->keyboard.left_btn);
        res.iaxis.y = res.faxis.y = IsKeyDown(pc->keyboard.down_btn)  - IsKeyDown(pc->keyboard.up_btn);

        res.press_axis.x = IsKeyPressed(pc->keyboard.right_btn) - IsKeyPressed(pc->keyboard.left_btn);
        res.press_axis.y = IsKeyPressed(pc->keyboard.down_btn)  - IsKeyPressed(pc->keyboard.up_btn);

        if (IsKeyPressed(pc->keyboard.super_btn)) {
            res.forw_btn = res.super_btn = true;
        }
        if (IsKeyPressed(pc->keyboard.strhit_btn)) {
            res.strhit_btn = true;
        }
        if (IsKeyPressed(pc->keyboard.uphit_btn)) {
            res.uphit_btn = true;
        }
    }

    return res;
}
