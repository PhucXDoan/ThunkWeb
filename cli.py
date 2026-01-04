#! /usr/bin/env python3

import deps.pxd.pxd as pxd



################################################################################



main_interface = pxd.CommandLineInterface()



################################################################################



@main_interface.new_verb(
    {
        'description' : 'Delete all build artifacts.',
    },
)
def clean(parameters):

    DIRECTORIES = (
        pxd.make_main_relative_path('./build'),
    )

    for directory in DIRECTORIES:

        pxd.execute_shell_command(
            bash = f'''
                rm -rf {repr(directory.as_posix())}
            ''',
            cmd = f'''
                if exist "{directory}" rmdir /S /Q "{directory}"
            ''',
        )



################################################################################



@main_interface.new_verb(
    {
        'description' : 'Compile and generate the executable.',
    },
)
def build(parameters):

    pxd.execute_shell_command(
        bash = f'mkdir -p {pxd.make_main_relative_path('./build')}',
        cmd  = f'''
            if not exist "{pxd.make_main_relative_path('./build')}" (
                mkdir {pxd.make_main_relative_path('./build')}
            )
        ''',
    )

    pxd.execute_shell_command('''
        cd ./deps/raylib && zig build
    ''')

    pxd.execute_shell_command('''
        cd build && zig cc ../source/ThunkWeb.c ../deps/raylib/zig-out/lib/raylib.lib -l gdi32 -l Winmm -o ./ThunkWeb.exe
    ''')



################################################################################



main_interface.invoke()
