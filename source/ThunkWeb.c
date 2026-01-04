#include "defs.h"
#include "raylib.h"

extern int
main(void)
{

    // Raylib initialization.

    InitWindow(800, 450, "raylib [core] example - basic window");

    SetTargetFPS(60);



    // Main loop.

    b32 quit = false;

    while (!quit)
    {

        // Handle inputs.

        quit |= WindowShouldClose();
        quit |= IsKeyDown(KEY_LEFT_CONTROL ) && IsKeyDown(KEY_W);
        quit |= IsKeyDown(KEY_RIGHT_CONTROL) && IsKeyDown(KEY_W);



        // Render.

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
            DrawRectangle(100, 200, 50, 90, RED);
        }
        EndDrawing();

    }



    return 0;

}
