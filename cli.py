#! /usr/bin/env python3



import pathlib
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
    {
        'name'        : 'metapreprocess_only',
        'description' : 'Run the meta-preprocessor; no compiling and linking.',
        'type'        : bool,
        'default'     : False,
    },
)
def build(parameters):



    # Determine the files for the meta-preprocessor to scan through.

    metapreprocessor_file_paths = [
        pathlib.Path(root, file_name)
        for root, directories, file_names in pxd.make_main_relative_path('./source').walk()
        for                    file_name  in file_names
        if file_name.endswith(('.c', '.h', '.py', '.ld', '.S'))
    ]



    # Begin meta-preprocessing!

    try:
        pxd.metapreprocess(
            output_directory_path = pxd.make_main_relative_path('./electrical/meta'),
            source_file_paths     = metapreprocessor_file_paths,
        )
    except pxd.MetaPreprocessorError:
        sys.exit(1)



    # Sometimes we only just need to run the meta-preprocessor without compiling.

    if parameters.metapreprocess_only:
        return



    # Create the build artifact folder.

    pxd.execute_shell_command(
        bash = f'mkdir -p {pxd.make_main_relative_path('./build')}',
        cmd  = f'''
            if not exist "{pxd.make_main_relative_path('./build')}" (
                mkdir {pxd.make_main_relative_path('./build')}
            )
        ''',
    )



    # Build the RayLib library.

    pxd.execute_shell_command('''
        cd ./deps/raylib && zig build
    ''')



    # Build the executable.

    pxd.execute_shell_command('''
        cd build &&
            zig cc
                ../source/ThunkWeb.c
                ../deps/raylib/zig-out/lib/raylib.lib
                -l gdi32
                -l Winmm
                -o ./ThunkWeb.exe
    ''')



################################################################################



main_interface.invoke()
