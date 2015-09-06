# Parallel Make
## Introduction
Make is an utility, which allows programmers (and not only programmers), to manage large projects, such that
when an edit to the project is made, only relevant part of the project is rebuild, redeployed or regenerated.
This is an implementation of make utility. For more info about make utility see
[this](http://www.gnu.org/software/make/manual/make.html) link.

## Supported platforms
This program can be run on any platform, which has C\+\+11 support, POSIX threads and POSIX getopt. It was tested on these platforms:<br>
*Windows 7*, with [wingetopt](https://github.com/alex85k/wingetopt),
[Pthreads Win32](https://www.sourceware.org/pthreads-win32/), compiled with either
[MVS2015](https://www.visualstudio.com/) or [MinGW](http://www.mingw.org/) G++ 4.9.2<br>
Debian 8.1, compiled with G++ 4.9.2<br>
FreeBSD 10.2-RELEASE, compiled with Clang++ 3.4.1

## Download
You can download source code directly as release of this repository, or you can clone the repository
yourself and build it. For building, see **Build** section.<br>
Release can not be directly downloaded.

## Build
#### Windows
Open `pmake.sln` in MVS2015, Select *Release* configuration and press *Build -> Build Solution*. Executable will be generated in `pmake\Release\pmake.exe`. For running, you have to download [pthread DLL](ftp://sourceware.org/pub/pthreads-win32/dll-latest/dll/x86/pthreadVCE2.dll).

#### Others
If you have GNU make installed. execute following command from `pmake` directory:<br>
`make`

If you don't have GNU make installed, execute following command from `pmake` directory:<br>
`g++ -std=c++11 -lpthread -o pmake src/pmake.cpp src/main.cpp src/makefile_record.cpp src/thread_manager.cpp`<br>
or simmilar according to your compiler.

## Installation
You can then run `make install` or `./pmake install` to copy `pmake` executable into `/usr/bin/` directory.

If you want to install `pmake.exe` on windows, you have to copy `pmake.exe` to folder, which is in `$PATH`. Also, you have to make sure, that `pthreadVCE2.dll` is locateable (for example, by being in `C:\Windows\System32`).

## Documentation
Online programmers documentation can be found on [this](http://www.zereges.cz/pmake/doc/) link.

If you want offline documentation, you can generate it via [DoxyGen](http://www.stack.nl/~dimitri/doxygen/).<br>
In `pmake` folder, execute `make documentation`.<br>
Resulting (HTML) documentation can be found in `./doc` folder. 

## Running program
`pmake` can be run by executing `.\pmake [options [...]] [target [...]]` from folder in which it is located. For information about options on command line, see next section.

## Description

    PMAKE
        pmake - parallel make utility to maintain groups of programs. For further
                information look at man pages for MAKE(1)

    SYNOPSIS
        pmake [OPTION]... [target]...

    OPTIONS
        -b, -m
            These options are ignored for compatibility with other versions of make.

        -B, --always-make
            Unconditionally make all targets.

        -d, --verbose
            Print debugging information about what is going on.

        -C dir, --directory=dir
            Change to directory dir before doing anything, Subsequent uses of this
            argument will be relative to previous one.

        -f file
            Use file as Makefile. If used multiple times, only last one will be
            active.

        -h, --help
            Prints help message.

        -j jobs, --jobs=jobs
            Specifies the number jobs (commands) to run simultaneously. This is
            implicitely on with jobs=number of cores.

        -n, --just-print, --dry-run, --recon
            Print the commands that would be executed, but do not execute them.

        -q, --question
            "Question mode". Do not run any commands, or print anything; just return
            an exit status that is zero if the specified targets are already up
            to date, one if targets need to be rebuilt and two is any error
            is encountered.

        -v, --version
            Print a message containing version and author. After the message is
            printed, pmake will exit.

        -W file, --what-if=file, --new-file=file, --assume-new=file
            Pretend that the target file has just been  modified. It is almost the
            same as running a touch command on the given file before running make,
            except that the modification time is changed only in the imagination of
            make.

        --warn-undefined-variables
            Warn when an undefined variable is referenced.

    MAKEFILE SYNTAX
        target [...] : [dependencies] [...]
            [command]
            [...]

        targets are name of files, which are generated as output of commands, while
        requiring dependencies. Command lines have to start with at least one white
        space character.
        Makefiles can also contain macros, which are defined as `variable = content`
        and used as `$(variable)`. Name of macro can only contain these
        characters: a-zA-Z0-9_-

        pmake will parse the Makefile from top to the bottom and will save macros
        when it encounters them. One macro can be defined as content of another, but
        no recurrence will be applied. Undefined macro contains empty string.

        pmake supports usage of special predefined macros. Here is list of them
        $@      Name of the first target
        $?      List of dependencies more recent than the target.
        $^      List of all dependencies without duplicities.
        $+      List of all dependencies in same order as they were specified.
        $<      First dependency.

    EXIT STATUS
        The pmake utility exits with status of zero if all makefiles were
        successfully parsed and no targets that were built failed. A status of one
        will be returned if the -q flag was used and make determines that a target
        needs to be rebuilt. A status of two will be returned if any errors were
        encountered.
