#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include "file.hpp"

//! Program return values. C style enum, so these are just int constants.
enum ret_values
{
    CODE_SUCCESS = 0, //!< Value, which is returned when make successfuly processes all given targets without failure.
    CODE_QUESTION_FAILURE = 1, //!< Value, which is returned if -q flag is specified, and there exists at least one target, which has to be rebuilt.
    CODE_FAILURE = 2, //!< Value, which is returned whenever an error occurs.
};

//! C++ like enum class. Return values of target processing.
enum class process_states
{
    MUST_REBUILD, //!< Value, returned whenever processed target was rebuilt.
    UP_TO_DATE, //!< Value, returned if processed target was up to date.
    QUESTION_FAILURE, //!< Value, returned if target has to be rebuild, but -q flag is specified.
    BUILD_FAILED, //!< Value, returned if one of executing commands failed.
    NOT_YET_PROCESSED, //< Value, returned if target is not yet processed.
    PROCESSING, //< Value, returned if target is currently in process.
};

//! Prints version and author.
inline void print_version()
{
    std::cout << "1.0, Filip Kliber <zereges@gmail.com>" << std::endl;
}

//! Prints basic help.
inline void print_help(const std::string& exe_name)
{
    std::cout << "Usage: " << exe_name << " [options] [target] ..." << std::endl <<
        R"(Options:
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
        Use file as Makefile.

    -h, --help
        Prints this error message includion version and author and exits with
        success.

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
        Warn when an undefined variable is referenced.)" << std::endl;
    std::cout << "For more information see GNU make manual." << std::endl << std::endl;
    print_version();
}

//! Converts string to a number with check if all chars of the string are 0-9.
//! This function is only used when converting -j flag, which has to be positive, so 0 is technicaly also error.
//! \param str string representing a number.
//! \return number represented by \p str, zero othrewise.
inline int to_number(const std::string& str)
{
    if (std::all_of(str.begin(), str.end(), [](decltype(*str.begin()) c) { return (c >= '0' && c <= '9'); }))
        return std::stoi(str);
    else
        return 0;
}

//! Replaces occurences of \p find by space separated \p replaces in \p what string.
//! \sa replace(std::string&, const std::string&, const std::string&)
//! \param what String to search in.
//! \param find Substring to look for.
//! \param replaces std::vector to be serialized and replaced for \p find.
//! \return reference to \p what.
std::string& replace(std::string& what, const std::string& find, const std::vector<file>& replaces);

//! Replaces occurences of \p find by \p replace in string \p what.
//! \param what String to search in.
//! \param find Substring to look for.
//! \param replace Substring to be replaced for \p find.
//! \return reference to \p what.
std::string& replace(std::string& what, const std::string& find, const std::string& replace);

//! Splits input string by delimiter.
//! \param str string to be split.
//! \param delim delimiter by which str should be split.
//! \return new std::vector of strings.
std::vector<std::string> split(const std::string& str, char delim);
