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

#include "defs.h"
#include "mathematics.c"



#define PIXELS_PER_METER          100.0f
#define CAMERA_MOVEMENT_DAMPENING 16.0f
#define CAMERA_MOVEMENT_SPEED     4.0f



extern i32
main(void)
{

    ////////////////////////////////////////////////////////////////////////////////
    //
    // Raylib initialization.
    //



    #define WINDOW_SIZE_X 800
    #define WINDOW_SIZE_Y 450

    InitWindow
    (
        WINDOW_SIZE_X,
        WINDOW_SIZE_Y,
        "ThunkWeb"
    );

    SetTargetFPS(60);

    b32 quit = false;

    while (!quit)
    {



        ////////////////////////////////////////////////////////////////////////////////
        //
        // General input handling.
        //



        f32 delta_time = GetFrameTime();

        quit |= WindowShouldClose();
        quit |= IsKeyDown(KEY_LEFT_CONTROL ) && IsKeyDown(KEY_W);
        quit |= IsKeyDown(KEY_RIGHT_CONTROL) && IsKeyDown(KEY_W);



        ////////////////////////////////////////////////////////////////////////////////
        //
        // Update camera.
        //



        static f32 camera_target_x = 0.0f;
        static f32 camera_target_y = 0.0f;
        static f32 camera_center_x = 0.0f;
        static f32 camera_center_y = 0.0f;

        f32 camera_control_x = 0.0f;
        f32 camera_control_y = 0.0f;

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT )) camera_control_x -= 1.0f;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) camera_control_x += 1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN )) camera_control_y -= 1.0f;
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP   )) camera_control_y += 1.0f;

        normalize(&camera_control_x, &camera_control_y);

        camera_target_x += camera_control_x * CAMERA_MOVEMENT_SPEED * delta_time;
        camera_target_y += camera_control_y * CAMERA_MOVEMENT_SPEED * delta_time;

        camera_center_x = damp(camera_center_x, camera_target_x, CAMERA_MOVEMENT_DAMPENING, delta_time);
        camera_center_y = damp(camera_center_y, camera_target_y, CAMERA_MOVEMENT_DAMPENING, delta_time);



        ////////////////////////////////////////////////////////////////////////////////
        //
        // Update slots.
        //



        struct Slot
        {
            f32       size_x;
            f32       size_y;
            f32       position_x;
            f32       position_y;
            b32       show_message_box;
            char      name[64];
            u16       dependencies[8];
            i32       dependency_count;
            Rectangle rectangle;
        };



        #include "slots.meta"
        /* #meta

            import types, math

            SLOTS = (
                types.SimpleNamespace(
                    name         = 'HSI_CK',
                    dependencies = (),
                ),
                types.SimpleNamespace(
                    name         = 'CSI_CK',
                    dependencies = (),
                ),
                types.SimpleNamespace(
                    name         = 'HSE_CK',
                    dependencies = (),
                ),
                types.SimpleNamespace(
                    name         = 'PLL_1_CLOCK_SOURCE',
                    dependencies = ('HSI_CK', 'CSI_CK', 'HSE_CK'),
                ),
                types.SimpleNamespace(
                    name         = 'PLL_1_PREDIVIDER',
                    dependencies = ('PLL_1_CLOCK_SOURCE',),
                ),
                types.SimpleNamespace(
                    name         = 'PLL_1_MULTIPLIER',
                    dependencies = ('PLL_1_PREDIVIDER',),
                ),
                types.SimpleNamespace(
                    name         = 'PLL_1_CHANNEL_P_DIVIDER',
                    dependencies = ('PLL_1_MULTIPLIER',),
                ),
                types.SimpleNamespace(
                    name         = 'PLL_1_CHANNEL_Q_DIVIDER',
                    dependencies = ('PLL_1_MULTIPLIER',),
                ),
                types.SimpleNamespace(
                    name         = 'PLL_1_CHANNEL_R_DIVIDER',
                    dependencies = ('PLL_1_MULTIPLIER',),
                ),
                types.SimpleNamespace(
                    name         = 'PLL_2_CLOCK_SOURCE',
                    dependencies = ('HSI_CK', 'CSI_CK', 'HSE_CK'),
                ),
                types.SimpleNamespace(
                    name         = 'PLL_2_PREDIVIDER',
                    dependencies = ('PLL_2_CLOCK_SOURCE',),
                ),
                types.SimpleNamespace(
                    name         = 'PLL_2_MULTIPLIER',
                    dependencies = ('PLL_2_PREDIVIDER',),
                ),
                types.SimpleNamespace(
                    name         = 'PLL_2_CHANNEL_P_DIVIDER',
                    dependencies = ('PLL_2_MULTIPLIER',),
                ),
                types.SimpleNamespace(
                    name         = 'PLL_2_CHANNEL_Q_DIVIDER',
                    dependencies = ('PLL_2_MULTIPLIER',),
                ),
                types.SimpleNamespace(
                    name         = 'PLL_2_CHANNEL_R_DIVIDER',
                    dependencies = ('PLL_2_MULTIPLIER',),
                ),
            )

            with Meta.enter('static struct Slot slots[] ='):

                for slot_i, slot in enumerate(SLOTS):

                    dependencies = [
                        next(
                            other_i
                            for other_i, other in enumerate(SLOTS)
                            if other.name == dependency
                        )
                        for dependency in slot.dependencies
                    ]

                    Meta.line(f'''
                        {{
                            .size_x           = {0.4 + len(slot.name) * 0.07 :.3f}f,
                            .size_y           = 0.3f,
                            .position_x       = {math.cos(slot_i / len(SLOTS) * math.tau) * 4 :.3f}f,
                            .position_y       = {math.sin(slot_i / len(SLOTS) * math.tau) * 4 :.3f}f,
                            .name             = "{slot.name}",
                            .dependencies     = {{ {', '.join(map(str, dependencies))} }},
                            .dependency_count = {len(dependencies)}
                        }},
                    ''')

        */

        for (i32 slot_i = 0; slot_i < countof(slots); slot_i += 1)
        {

            struct Slot* slot = &slots[slot_i];

            slot->rectangle =
                (Rectangle)
                {
                    .x      =                     (slot->position_x - slot->size_x * 0.5f - camera_center_x) * PIXELS_PER_METER + WINDOW_SIZE_X / 2,
                    .y      = WINDOW_SIZE_Y / 2 - (slot->position_y + slot->size_y * 0.5f - camera_center_y) * PIXELS_PER_METER,
                    .width  = slot->size_x * PIXELS_PER_METER,
                    .height = slot->size_y * PIXELS_PER_METER,
                };

        }



        ////////////////////////////////////////////////////////////////////////////////
        //
        // Render.
        //



        BeginDrawing();
        {

            ClearBackground(RAYWHITE);

            GuiGrid
            (
                (Rectangle)
                {
                    .x      = -camera_center_x * PIXELS_PER_METER + WINDOW_SIZE_X / 2,
                    .y      =  camera_center_y * PIXELS_PER_METER + WINDOW_SIZE_Y / 2,
                    .width  = WINDOW_SIZE_X,
                    .height = WINDOW_SIZE_Y,
                },
                "meow",
                PIXELS_PER_METER,
                8,
                nullptr
            );

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
                struct Slot* current_slot = &slots[slot_i    ];

                for (i32 dependency_i = 0; dependency_i < current_slot->dependency_count; dependency_i += 1)
                {
                    struct Slot* dependency_slot = &slots[current_slot->dependencies[dependency_i]];

                    Color color = BLACK;

                    if
                    (
                        !GuiIsLocked() &&
                        (
                            CheckCollisionPointRec(GetMousePosition(), current_slot->rectangle   ) ||
                            CheckCollisionPointRec(GetMousePosition(), dependency_slot->rectangle)
                        )
                    )
                    {
                        color = BLUE;
                    }

                    DrawLine
                    (
                        (int) (current_slot->rectangle.x    + current_slot->rectangle.width     * 0.5f),
                        (int) (current_slot->rectangle.y    + current_slot->rectangle.height    * 0.5f),
                        (int) (dependency_slot->rectangle.x + dependency_slot->rectangle.width  * 0.5f),
                        (int) (dependency_slot->rectangle.y + dependency_slot->rectangle.height * 0.5f),
                        color
                    );

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
                    "%s %d",
                    slot->name,
                    slot_i
                );



                b32 button_pressed = GuiButton(slot->rectangle, button_text_buffer);

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
                                .x      = slot->rectangle.x + 25,
                                .y      = slot->rectangle.y + 25,
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
