#include "raylib.h"
#include "config.h"
#include "game.h"

int main(void)
{
//    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WIDTH, HEIGHT, TITLE);
    SetExitKey(KEY_NULL);
    SetTargetFPS(FPS);

    InitAudioDevice();

    Game game;
    init_game(&game);

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();

        update_game(&game, dt);

        BeginDrawing();
            draw_game(&game);
        EndDrawing();
    }

    CloseAudioDevice();

    // close game
    CloseWindow();

    return 0;
}
