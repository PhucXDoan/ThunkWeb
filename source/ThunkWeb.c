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



static f32
lerp(f32 a, f32 b, f32 t)
{
    return a * (1.0f - t) + b * t;
}

static f32
damp(f32 a, f32 b, f32 k, f32 dt)
{
    return lerp(a, b, 1 - expf(-k * dt));
}



extern i32
main(void)
{

    // Raylib initialization.

    #define WINDOW_SIZE_X 800
    #define WINDOW_SIZE_Y 450

    InitWindow
    (
        WINDOW_SIZE_X,
        WINDOW_SIZE_Y,
        "ThunkWeb"
    );

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
            f32  size_x;
            f32  size_y;
            f32  position_x;
            f32  position_y;
            f32  velocity_x;
            f32  velocity_y;
            b32  show_message_box;
            char name[64];
            u16  dependencies[8];
            i32  dependency_count;
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
                            .size_x           = {40 + len(slot.name) * 7},
                            .size_y           = 30.0f,
                            .position_x       = {round(400 + math.cos(slot_i) * 150)},
                            .position_y       = {round(200 + math.sin(slot_i) * 150)},
                            .velocity_x       = 0.0f,
                            .velocity_y       = 0.0f,
                            .name             = "{slot.name}",
                            .dependencies     = {{ {', '.join(map(str, dependencies))} }},
                            .dependency_count = {len(dependencies)}
                        }},
                    ''')

        */

        for (i32 slot_i = 0; slot_i < countof(slots); slot_i += 1)
        {

            struct Slot* slot = &slots[slot_i];

            slot->position_x += slot->velocity_x * GetFrameTime();
            slot->position_y += slot->velocity_y * GetFrameTime();

            slot->velocity_x = damp(slot->velocity_x, 0.0f, 0.95f, GetFrameTime());
            slot->velocity_y = damp(slot->velocity_y, 0.0f, 0.95f, GetFrameTime());

            f32 dispersion_x = 0.0f;
            f32 dispersion_y = 0.0f;

            for (i32 slot_j = 0; slot_j < countof(slots); slot_j += 1)
            {
                struct Slot* other = &slots[slot_j];

                f32 delta_x  = other->position_x - slot->position_x;
                f32 delta_y  = other->position_y - slot->position_y;
                f32 distance = sqrtf(delta_x * delta_x + delta_y * delta_y);

                if (distance >= 0.001f)
                {
                    f32 direction_x = delta_x / distance;
                    f32 direction_y = delta_y / distance;
                    f32 weight      = 100'000.0f / (distance * distance);

                    dispersion_x += -direction_x * weight;
                    dispersion_y += -direction_y * weight;
                }
            }

            slot->velocity_x += dispersion_x * GetFrameTime();
            slot->velocity_y += dispersion_y * GetFrameTime();



            f32 convergence_x = 0.0f;
            f32 convergence_y = 0.0f;

            for (i32 dependency_i = 0; dependency_i < slot->dependency_count; dependency_i += 1)
            {
                struct Slot* other = &slots[slot->dependencies[dependency_i]];

                f32 delta_x  = other->position_x - slot->position_x;
                f32 delta_y  = other->position_y - slot->position_y;
                f32 distance = sqrtf(delta_x * delta_x + delta_y * delta_y);

                if (distance >= 0.001f)
                {
                    f32 direction_x = delta_x / distance;
                    f32 direction_y = delta_y / distance;
                    f32 weight      = 100'000.0f / (10.0f + (distance - 300.0f) * (distance - 300.0f));

                    convergence_x += direction_x * weight;
                    convergence_y += direction_y * weight;
                }
            }

            slot->velocity_x += convergence_x * GetFrameTime();
            slot->velocity_y += convergence_y * GetFrameTime();



            if (slot->position_x - slot->size_x * 0.5f < 0.0f)
            {
                slot->position_x = slot->size_x * 0.5f;
                slot->velocity_x = fabsf(slot->velocity_x);
            }

            if (slot->position_x + slot->size_x * 0.5f > WINDOW_SIZE_X)
            {
                slot->position_x  = WINDOW_SIZE_X - slot->size_x * 0.5f;
                slot->velocity_x = -fabsf(slot->velocity_x);
            }

            if (slot->position_y - slot->size_y * 0.5f < 0.0f)
            {
                slot->position_y  = slot->size_y * 0.5f;
                slot->velocity_y = fabsf(slot->velocity_y);
            }

            if (slot->position_y + slot->size_y * 0.5f > WINDOW_SIZE_Y)
            {
                slot->position_y = WINDOW_SIZE_Y - slot->size_y * 0.5f;
                slot->velocity_y = -fabsf(slot->velocity_y);
            }

        }



        // Render.

        BeginDrawing();
        {

            ClearBackground(RAYWHITE);

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
                            CheckCollisionPointRec
                            (
                                GetMousePosition(),
                                (Rectangle)
                                {
                                    .x      = current_slot->position_x - current_slot->size_x * 0.5f,
                                    .y      = current_slot->position_y - current_slot->size_y * 0.5f,
                                    .width  = current_slot->size_x,
                                    .height = current_slot->size_y,
                                }
                            ) ||
                            CheckCollisionPointRec
                            (
                                GetMousePosition(),
                                (Rectangle)
                                {
                                    .x      = dependency_slot->position_x - dependency_slot->size_x * 0.5f,
                                    .y      = dependency_slot->position_y - dependency_slot->size_y * 0.5f,
                                    .width  = dependency_slot->size_x,
                                    .height = dependency_slot->size_y,
                                }
                            )
                        )
                    )
                    {
                        color = BLUE;
                    }

                    DrawLine
                    (
                        (i32) current_slot->position_x,
                        (i32) current_slot->position_y,
                        (i32) dependency_slot->position_x,
                        (i32) dependency_slot->position_y,
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



                b32 button_pressed =
                    GuiButton
                    (
                        (Rectangle)
                        {
                            .x      = slot->position_x - slot->size_x * 0.5f,
                            .y      = slot->position_y - slot->size_y * 0.5f,
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
