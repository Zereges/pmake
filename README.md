```
PMAKE
    pmake - parallel make utility to maintain groups of programs. For further
            information look at man pages for MAKE(1)

SYNOPSIS
    pmake [OPTION]... [target]...

OPTIONS
    -b, -m
        These options are ignored for compatibility with other versions of make.

    -B, --always-make
        Unconfitionally make all targets.

    -d, --verbose
        Print debugging information about what is going on.

    -C dir, --directory=dir
        Change to directory dir before doing anything, Sunsequent uses of this
        argument will be relative to previous one.

    -f file
        Use file as Makefile.

    -j [jobs], --jobs=jobs
        Specifies the number jobs (commands) to run simultaneously. If -j is
        given without argument pmake utility will not limit the number of jobs
        that can run simultaneously. This is implicitely on with jobs=number of
        cores.

    -n, --just-print, --dry-run, --recon
        Print the commands that would be executed, but do not execute them.

    -q, --question
        "Question mode". Do not run any commands, or print anything; just return
        an exit status that is zero if the specified targets are already up
        to date, nonzero otherwise.

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
    target... : [dependencies]...
    \tab    command
    \tab    ...

    targets are name of files, which are generated as output of commands, while
    requiring prerequisities. Makefiles can also contain macros, which are
    defined as `variable = content` and used as `$(variable)`. Name of macro can
    only contain these characters: a-zA-Z0-9_-

    pmake will parse the Makefile from top to the bottom and will save macros
    when it encounters them. One macro can be defined as content of another, but
     no recurrence will be applied. Undefined macro contains empty string.

    pmake supports usage of special predefined macros. Here is list of them
    $@      Name of the target
    $?      List of dependencies more recent than the target.
    $^      List of all dependencies without duplicities.
    $+      List of all dependencies in same order as they were specified.
    $<      First dependency.
    $*      Reference to %, see below for details.

    pmake also supports generic targets. For example
    %.o: %.cpp
        echo $*
    rule suits any target ending with %.o and having same dependency with
    different suffix. Invoking `make test.o` while having `test.cpp` in give
    directory will print `test` to stdout.


EXIT STATUS
    The pmake utility exitst with status of zero if all makefiles were
    successfully parsed and no targets that were built failed. A status of one
    will be returned if the -q flag was used and  make determines that a target
    needs to be rebuilt. A status of two will be returned if any errors were
    encountered.
```
