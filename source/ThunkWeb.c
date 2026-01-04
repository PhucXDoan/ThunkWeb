#include "defs.h"
#include "raylib.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#pragma clang diagnostic ignored "-Wbad-function-cast"
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#pragma clang diagnostic pop



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

            Vector2 box_center = { 100, 200 };
            Vector2 box_size   = { 150, 90 };

            DrawRectangle
            (
                (i32) (box_center.x - box_size.x / 2),
                (i32) (box_center.y - box_size.y / 2),
                (i32) box_size.x,
                (i32) box_size.y,
                RED
            );

            DrawText
            (
                "Congrats! You created your first window!",
                (i32) box_center.x,
                (i32) box_center.y,
                16,
                LIGHTGRAY
            );



            static b32 showMessageBox = false;

            if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#191#Show Message"))
            {
                showMessageBox = true;
            }

            if (showMessageBox)
            {
                int result = GuiMessageBox((Rectangle){ 85, 70, 250, 100 },
                    "#191#Message Box", "Hi! This is a message!", "Nice;Cool");

                if (result >= 0) showMessageBox = false;
            }

        }
        EndDrawing();

    }



    return 0;

}
