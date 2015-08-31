#pragma once
#include <iostream>
#include <string>
#include <algorithm>

enum ret_values
{
    CODE_SUCCESS = 0,
    CODE_QUESTION_FAILURE = 1,
    CODE_FAILURE = 2,
};

inline void print_version()
{
    std::cout << "1.0, Filip Kliber <zereges@gmail.com>" << std::endl;
}

inline void print_help(const std::string& exe_name)
{
    std::cout << "Usage: " << exe_name << " [options] [target] ..." << std::endl <<
        R"--(Options:
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

            -j jobs, --jobs=jobs
        Specifies the number jobs (commands) to run simultaneously. This is
        implicitely on with jobs=number of cores.

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
        Warn when an undefined variable is referenced.)--" << std::endl << std::endl;

    print_version();
}

inline int to_number(const std::string& str)
{
    if (std::all_of(str.begin(), str.end(), [](decltype(*str.begin()) c) { return (c >= '0' && c <= '9'); }))
        return std::stoi(str);
    else
        return 0;
}
