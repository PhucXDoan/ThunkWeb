#! /usr/bin/env python3

from pxd import logger, make_main_relative_path, execute_shell_command, CommandLineInterface



################################################################################



main_interface = CommandLineInterface()



################################################################################



@main_interface.new_verb(
    {
        'description' : 'Delete all build artifacts.',
    },
)
def clean(parameters):

    DIRECTORIES = (
        make_main_relative_path('./build'),
    )

    for directory in DIRECTORIES:

        execute_shell_command(
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

    execute_shell_command(
        bash = f'mkdir -p {make_main_relative_path('./build')}',
        cmd  = f'''
            if not exist "{make_main_relative_path('./build')}" (
                mkdir {make_main_relative_path('./build')}
            )
        ''',
    )

    execute_shell_command('''
        cd ./submodules/raylib && zig build
    ''')

    execute_shell_command('''
        cd build && zig cc ../source/ThunkWeb.c ../submodules/raylib/zig-out/lib/raylib.lib -l gdi32 -l Winmm -o ./ThunkWeb.exe
    ''')



################################################################################



main_interface.invoke()
