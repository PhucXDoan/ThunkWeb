#include <assert.h>
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



#define CAMERA_MOVEMENT_DAMPENING      16.0f
#define CAMERA_MOVEMENT_SPEED_FACTOR   4.0f
#define CAMERA_ZOOM_SCROLL_SENSITIVITY 0.5f



static void
allocate_and_read_file(char* file_path, u8** file_data, i64* file_length)
{

    // Get stuff ready.

    assert(file_path);
    assert(file_data);
    assert(file_length);

    i32 integer_result = {0};

    *file_data   = nullptr;
    *file_length = 0;



    // Get file handle.

    FILE* file_handle = fopen(file_path, "rb");
    assert(file_handle);



    // Get file length.

    integer_result = fseek(file_handle, 0, SEEK_END);
    assert(!integer_result);

    *file_length = ftell(file_handle);
    assert(*file_length != -1);

    integer_result = fseek(file_handle, 0, SEEK_SET);
    assert(!integer_result);



    // Get file content.

    if (*file_length)
    {
        *file_data = calloc((u32) *file_length, sizeof(u8));
        assert(*file_data);

        u64 fread_result = fread(*file_data, (u64) *file_length, 1, file_handle);
        assert(fread_result == 1);
    }



    // Done with file handle.

    integer_result = fclose(file_handle);
    assert(!integer_result);

}



static void
load_info_file(void)
{

    ////////////////////////////////////////////////////////////////////////////////
    //
    // TODO.
    //



    u8* file_data   = {0};
    i64 file_length = {0};
    allocate_and_read_file("./info/STM32H533RET6.info", &file_data, &file_length);

    printf("```\n%.*s\n```\n", (i32) file_length, file_data);



    i64 file_reader = 0;

    while (file_reader < file_length)
    {

        // Skip whitespace.

        if (file_data[file_reader] <= 32)
        {
            file_reader += 1;
            continue;
        }



        // Grab the identifier.

        u8* identifier_data   = &file_data[file_reader];
        i64 identifier_length = 0;

        while (true)
        {
            if (file_reader == file_length)
            {
                break;
            }
            else if (file_data[file_reader] <= 32)
            {
                file_reader += 1;
                break;
            }
            else
            {
                file_reader       += 1;
                identifier_length += 1;
            }
        }



        // Determine the keyword.

        #include "InfoKeyword.meta"
        /* #meta

            KEYWORDS = (
                'name',
                'position_x',
                'position_y',
                'dependencies',
            )

            Meta.enums('InfoKeyword', 'u32', KEYWORDS)
            Meta.lut('INFO_KEYWORDS', (
                (
                    f'InfoKeyword_{keyword}',
                    ('const char*', 'text', f'"{keyword}"'),
                )
                for keyword in KEYWORDS
            ))

        */

        b32              valid_keyword = false;
        enum InfoKeyword keyword       = {0};

        for (enum InfoKeyword it = {0}; it < InfoKeyword_COUNT; it += 1)
        {
            if
            (
                strncmp
                (
                    INFO_KEYWORDS[it].text,
                    (const char*) identifier_data,
                    (u64        ) identifier_length
                ) == 0
            )
            {
                valid_keyword = true;
                keyword       = it;
                break;
            }
        }

        if (!valid_keyword)
        {
            printf("Non-keyword `%.*s`\n", (i32) identifier_length, identifier_data);
            continue; // TODO.
        }



        // Handle keyword.

        switch (keyword)
        {
            case InfoKeyword_name:
            {
                printf("Name `%.*s`\n", (i32) identifier_length, identifier_data);
            } break;

            case InfoKeyword_position_x:
            {
                printf("Position-x `%.*s`\n", (i32) identifier_length, identifier_data);
            } break;

            case InfoKeyword_position_y:
            {
                printf("Position-y `%.*s`\n", (i32) identifier_length, identifier_data);
            } break;

            case InfoKeyword_dependencies:
            {
                printf("Dependencies `%.*s`\n", (i32) identifier_length, identifier_data);
            } break;

            default: assert(false); // TODO.
        }

    }

}



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

    load_info_file();

    b32 quit = false;

    while (!quit)
    {



        ////////////////////////////////////////////////////////////////////////////////
        //
        // General input handling.
        //



        f32 delta_time = GetFrameTime();

        quit |= WindowShouldClose();
        quit |= IsKeyDown(KEY_LEFT_CONTROL ) && IsKeyPressed(KEY_W);
        quit |= IsKeyDown(KEY_RIGHT_CONTROL) && IsKeyPressed(KEY_W);



        ////////////////////////////////////////////////////////////////////////////////
        //
        // Update camera zoom.
        //



        static f32 camera_zoom_target   = 0.0f;
        static f32 camera_zoom_exponent = 0.0f;

        f32 camera_zoom_direction = 0.0f;

        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_MINUS))
        {
            camera_zoom_direction -= 1.0f;
        }

        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_EQUAL))
        {
            camera_zoom_direction += 1.0f;
        }

        camera_zoom_direction += GetMouseWheelMove() * CAMERA_ZOOM_SCROLL_SENSITIVITY;

        camera_zoom_target   += camera_zoom_direction * 0.5f;
        camera_zoom_target    = minf(maxf(camera_zoom_target, -1.0f), 1.5f);
        camera_zoom_exponent  = damp(camera_zoom_exponent, camera_zoom_target, 8.0f, delta_time);

        f32 pixels_per_meter = 100.0f * expf(camera_zoom_exponent);



        ////////////////////////////////////////////////////////////////////////////////
        //
        // Update camera view.
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

        camera_target_x += camera_control_x * expf(-camera_zoom_exponent) * CAMERA_MOVEMENT_SPEED_FACTOR * delta_time;
        camera_target_y += camera_control_y * expf(-camera_zoom_exponent) * CAMERA_MOVEMENT_SPEED_FACTOR * delta_time;

        camera_center_x = damp(camera_center_x, camera_target_x, CAMERA_MOVEMENT_DAMPENING, delta_time);
        camera_center_y = damp(camera_center_y, camera_target_y, CAMERA_MOVEMENT_DAMPENING, delta_time);

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
        {

            Vector2 delta = GetMouseDelta();
            delta.x /= pixels_per_meter;
            delta.y /= pixels_per_meter;

            camera_target_x -= delta.x;
            camera_target_y += delta.y;
            camera_center_x -= delta.x;
            camera_center_y += delta.y;

        }



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
            import deps.pxd.pxd as pxd

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



            info = ''

            for slot_i, slot in enumerate(SLOTS):

                dependencies = [
                    next(
                        other_i
                        for other_i, other in enumerate(SLOTS)
                        if other.name == dependency
                    )
                    for dependency in slot.dependencies
                ]

                info += f'name         {slot.name}'                                         '\n'
                info += f'position_x   {math.cos(slot_i / len(SLOTS) * math.tau) * 4 :.3f}' '\n'
                info += f'position_y   {math.sin(slot_i / len(SLOTS) * math.tau) * 4 :.3f}' '\n'
                if dependencies:
                    info += f'dependencies {' '.join(map(str, dependencies))}'                  '\n'
                info += '\n'

            pxd.make_main_relative_path('./info/STM32H533RET6.info').write_text(info)

        */

        for (i32 slot_i = 0; slot_i < countof(slots); slot_i += 1)
        {

            struct Slot* slot = &slots[slot_i];

            slot->rectangle =
                (Rectangle)
                {
                    .x      =                     (slot->position_x - slot->size_x * 0.5f - camera_center_x) * pixels_per_meter + WINDOW_SIZE_X / 2,
                    .y      = WINDOW_SIZE_Y / 2 - (slot->position_y + slot->size_y * 0.5f - camera_center_y) * pixels_per_meter,
                    .width  = slot->size_x * pixels_per_meter,
                    .height = slot->size_y * pixels_per_meter,
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
                    .x      = -camera_center_x * pixels_per_meter + WINDOW_SIZE_X / 2,
                    .y      =  camera_center_y * pixels_per_meter + WINDOW_SIZE_Y / 2,
                    .width  = WINDOW_SIZE_X,
                    .height = WINDOW_SIZE_Y,
                },
                "meow",
                pixels_per_meter,
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
