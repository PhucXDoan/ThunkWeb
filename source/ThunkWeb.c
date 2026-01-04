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
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#pragma clang diagnostic pop



extern i32
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



        // TODO.

        struct Slot
        {
            f32 size_x;
            f32 size_y;
            f32 position_x;
            f32 position_y;
            f32 velocity_x;
            f32 velocity_y;
            b32 show_message_box;
        };

        static struct Slot slots[] =
            {
                {
                    .size_x     = 120.0f,
                    .size_y     = 30.0f,
                    .position_x = 10.0f,
                    .position_y = 20.0f,
                    .velocity_x = 5.0f,
                    .velocity_y = 3.0f,
                },
                {
                    .size_x     = 170.0f,
                    .size_y     = 20.0f,
                    .position_x = 50.0f,
                    .position_y = 40.0f,
                    .velocity_x = -5.0f,
                    .velocity_y = 8.0f,
                },
                {
                    .size_x     = 170.0f,
                    .size_y     = 20.0f,
                    .position_x = 150.0f,
                    .position_y = 140.0f,
                    .velocity_x = 8.0f,
                    .velocity_y = -4.0f,
                },
            };

        for (i32 slot_i = 0; slot_i < countof(slots); slot_i += 1)
        {
            slots[slot_i].position_x += slots[slot_i].velocity_x * GetFrameTime();
            slots[slot_i].position_y += slots[slot_i].velocity_y * GetFrameTime();
        }



        // Render.

        BeginDrawing();
        {

            ClearBackground(RAYWHITE);

            for (i32 slot_i = 0; slot_i < countof(slots) - 1; slot_i += 1)
            {
                struct Slot* current_slot = &slots[slot_i    ];
                struct Slot* next_slot    = &slots[slot_i + 1];

                DrawLine
                (
                    (i32) (current_slot->position_x + current_slot->size_x * 0.5f),
                    (i32) (current_slot->position_y + current_slot->size_y * 0.5f),
                    (i32) (next_slot->position_x    + next_slot->size_x    * 0.5f),
                    (i32) (next_slot->position_y    + next_slot->size_y    * 0.5f),
                    BLACK
                );
            }


            for (i32 slot_i = 0; slot_i < countof(slots); slot_i += 1)
            {
                struct Slot* slot = &slots[slot_i];

                if (slot->show_message_box)
                {
                    GuiLock();
                }
            }

            for (i32 slot_i = 0; slot_i < countof(slots); slot_i += 1)
            {

                struct Slot* slot = &slots[slot_i];

                char button_text_buffer[32] = {0};

                snprintf
                (
                    button_text_buffer,
                    countof(button_text_buffer),
                    "Meow %d",
                    slot_i
                );



                b32 button_pressed =
                    GuiButton
                    (
                        (Rectangle)
                        {
                            .x      = slot->position_x,
                            .y      = slot->position_y,
                            .width  = slot->size_x,
                            .height = slot->size_y,
                        },
                        button_text_buffer
                    );

                if (button_pressed)
                {
                    slot->show_message_box = true;
                }

            }

            GuiUnlock();

            for (i32 slot_i = 0; slot_i < countof(slots); slot_i += 1)
            {
                struct Slot* slot = &slots[slot_i];

                if (slot->show_message_box)
                {
                    i32 result =
                        GuiMessageBox
                        (
                            (Rectangle)
                            {
                                .x      = slot->position_x + 25,
                                .y      = slot->position_y + 25,
                                .width  = 250,
                                .height = 100,
                            },
                            "#191#Message Box",
                            "Hi! This is a message!",
                            "Nice;Cool"
                        );

                    if (result >= 0)
                    {
                        slot->show_message_box = false;
                    }
                }
            }

        }
        EndDrawing();

    }



    return 0;

}
