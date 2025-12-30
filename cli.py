#! /usr/bin/env python3



#
# Enforce Python version.
#



import sys

MINIMUM_MAJOR = 3
MINIMUM_MINOR = 13

if not (
    sys.version_info.major == MINIMUM_MAJOR and
    sys.version_info.minor >= MINIMUM_MINOR
):
    raise RuntimeError(
        'Unsupported Python version: ' + repr(sys.version) + '; ' +
        'please upgrade to at least ' + str(MINIMUM_MAJOR) + '.' + str(MINIMUM_MINOR) + '; '
        'note that it is possible that you have multiple instances of Python installed; '
        'in this case, please set your PATH accordingly or use a Python virtual environment.'
    )



#
# Built-in modules.
#



import types, shlex, pathlib, shutil, subprocess, time, logging



#
# Logger configuration.
#



class MainFormatter(logging.Formatter):

    def format(self, record):



        message = super().format(record)



        # The `table` property allows for a
        # simple, justified table to be outputted.

        if hasattr(record, 'table'):

            for just_key, just_value in justify([
                (
                    ('<' , str(key  )),
                    (None, str(value)),
                )
                for key, value in record.table
            ]):
                message += f'\n{just_key} : {just_value}'



        # The main interface logger won't have its
        # info logs be prepended with the level.

        if not (
            record.name.split('.')[:2] == [__name__, 'main_interface']
            and record.levelname == 'INFO'
        ):



            # Any newlines will be indented so it'll look nice.

            indent = ' ' * len(f'[{record.levelname}] ')

            message = '\n'.join([
                message.splitlines()[0],
                *[f'{indent}{line}' for line in message.splitlines()[1:]]
            ])



            # Prepend the log level name and color based on severity.

            coloring = {
                'DEBUG'    : '\x1B[0;35m',
                'INFO'     : '\x1B[0;36m',
                'WARNING'  : '\x1B[0;33m',
                'ERROR'    : '\x1B[0;31m',
                'CRITICAL' : '\x1B[1;31m',
            }[record.levelname]

            reset = '\x1B[0m'

            message = f'{coloring}[{record.levelname}]{reset} {message}'



        # Give each log a bit of breathing room.

        message += '\n'



        return message



logger         = logging.getLogger(__name__)
logger_handler = logging.StreamHandler(sys.stdout)
logger_handler.setFormatter(MainFormatter())
logger.addHandler(logger_handler)
logger.setLevel(logging.DEBUG)



#
# Routine to carry out shell commands.
#



class ExecuteShellCommandNonZeroExitCode(Exception):
    pass



def execute_shell_command(
    default    = None,
    *,
    bash       = None,
    cmd        = None,
    powershell = None,
):



    # PowerShell is slow to invoke, so cmd.exe
    # would be used if its good enough.

    if cmd is not None and powershell is not None:
        raise ValueError('CMD and PowerShell commands cannot be both provided.')

    match sys.platform:

        case 'win32':
            use_powershell = cmd is None and powershell is not None
            commands       = powershell if use_powershell else cmd

        case _:
            commands       = bash
            use_powershell = False

    if commands is None:
        commands = default

    if commands is None:
        raise ValueError(f'Missing shell command for platform {repr(sys.platform)}.')

    if isinstance(commands, str):
        commands = [commands]



    # Process each command to have it be split into shell tokens.
    # The lexing that's done here is to do a lot of the funny
    # business involving escaping quotes and what not. To be honest,
    # it's a little out my depth, mainly because I frankly do not
    # care enough to get it 100% correct; it working most of the time
    # is good enough for me.

    for command_i in range(len(commands)):

        lexer                  = shlex.shlex(commands[command_i])
        lexer.quotes           = '"'
        lexer.whitespace_split = True
        lexer.commenters       = ''
        commands[command_i]    = list(lexer)



    # Execute each shell command.

    processes = []

    for command_i, command in enumerate(commands):

        command = ' '.join(command)

        logger.info(f'$ {command}')

        if use_powershell:

            # On Windows, Python will call CMD.exe
            # to run the shell command, so we'll
            # have to invoke PowerShell to run the
            # command if PowerShell is needed.

            processes += [subprocess.Popen(['pwsh', '-Command', command], shell = False)]

        else:

            processes += [subprocess.Popen(command, shell = True)]



    # Wait on each subprocess to be done.

    for process in processes:
        if process.wait():
            raise ExecuteShellCommandNonZeroExitCode



#
# TODO.
#



execute_shell_command('''
    cd ./submodules/raylib && zig build
''')

execute_shell_command('''
    cd build && zig cc ../source/ThunkWeb.c ../submodules/raylib/zig-out/lib/raylib.lib -l gdi32 -l Winmm -o ./ThunkWeb.exe
''')
