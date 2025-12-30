#! /usr/bin/env python3

from pxd import logger, execute_shell_command

execute_shell_command('''
    cd ./submodules/raylib && zig build
''')

execute_shell_command('''
    cd build && zig cc ../source/ThunkWeb.c ../submodules/raylib/zig-out/lib/raylib.lib -l gdi32 -l Winmm -o ./ThunkWeb.exe
''')
